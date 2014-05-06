<?php

/**
 * @file
 * Handles the uploading of the PDF file as part of the ingest process.
 */

/**
 * Defines a file upload form for uploading the tei file.
 *
 * @param array $form
 *   The drupal form.
 * @param array $form_state
 *   The drupal form state.
 *
 * @return array
 *   The drupal form definition.
 */
function islandora_born_digital_tei_upload_form(array $form, array &$form_state) {
  $upload_size = min((int) ini_get('post_max_size'), (int) ini_get('upload_max_filesize'));
  $extensions = array('xml txt');
  $form = array();
  $form['file'] = array(
    '#title' => t('TEI File'),
    '#type' => 'managed_file',
    '#required' => TRUE,
    '#description' => t('Select file to upload.<br/>Files must be less than <b>@size MB.</b><br/>Allowed file types: <b>@ext.</b>', array('@size' => $upload_size, '@ext' => $extensions[0])),
    '#default_value' => isset($form_state['values']['files']) ? $form_state['values']['files'] : NULL,
    '#upload_location' => 'temporary://',
    '#upload_validators' => array(
      'file_validate_extensions' => $extensions,
      // Assume it's specified in MB.
      'file_validate_size' => array($upload_size * 1024 * 1024),
    ),
  );

  $form['islandora_born_digital_text_upload'] = array(
    '#type' => 'checkbox',
    '#title' => t("Add text file to this upload?"),
    '#default_value' => FALSE,
  );
  $form['text'] = array(
    '#title' => t('Plain text transcription'),
    '#type' => 'managed_file',
    '#required' => FALSE,
    '#description' => t('Select text file to upload.<br/>Files must be less than <b>@size MB.</b><br/>Allowed file types: <b>@ext.</b><br />This file is optional.', array('@size' => $upload_size, '@ext' => 'txt')),
    '#default_value' => isset($form_state['values']['files']) ? $form_state['values']['files'] : NULL,
    '#upload_location' => 'temporary://',
    '#upload_validators' => array(
      'file_validate_extensions' => array('txt'),
      // Assume it's specified in MB.
      'file_validate_size' => array($upload_size * 1024 * 1024),
    ),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_born_digital_text_upload"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['islandora_born_digital_tn_upload'] = array(
    '#type' => 'checkbox',
    '#title' => t("Add optional thumbnail?"),
    '#default_value' => FALSE,
  );
  $form['tn'] = array(
    '#title' => t('Thumbnail for object'),
    '#type' => 'managed_file',
    '#required' => FALSE,
    '#description' => t('Select text file to upload.<br/>Files must be less than <b>@size MB.</b><br/>Allowed file types: <b>@ext.</b><br />This file is optional.', array('@size' => $upload_size, '@ext' => 'txt')),
    '#default_value' => isset($form_state['values']['files']) ? $form_state['values']['files'] : NULL,
    '#upload_location' => 'temporary://',
    '#upload_validators' => array(
      'file_validate_extensions' => array('jpg jpeg png'),
      // Assume it's specified in MB.
      'file_validate_size' => array($upload_size * 1024 * 1024),
    ),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_born_digital_tn_upload"]' => array('checked' => TRUE),
      ),
    ),
  );

  return $form;
}

/**
 * Addes the uploaded file into the ingestable objects 'OBJ' datastream.
 *
 * @param array $form
 *   The drupal form.
 * @param array $form_state
 *   The drupal form state.
 */
function islandora_born_digital_tei_upload_form_submit(array $form, array &$form_state) {
  $object = $form_state['islandora']['objects'][0];
  if (empty($object['OBJ'])) {
    $ds = $object->constructDatastream('OBJ', 'M');
    $object->ingestDatastream($ds);
  }
  else {
    $ds = $object['OBJ'];
  }
  $tei_file = file_load($form_state['values']['file']);
  $tei_path = drupal_realpath($tei_file->uri);
  $ds->label = $tei_file->filename;
  $ds->mimetype = "text/plain";
  $ds->setContentFromFile($tei_path, FALSE);

  if (isset($form_state['values']['text']) && ($form_state['values']['text'] > 0)) {
    if (empty($object['FULL_TEXT'])) {
      $ds = $object->constructDatastream('FULL_TEXT', 'M');
      $object->ingestDatastream($ds);
    }
    else {
      $ds = $object['FULL_TEXT'];
    }
    $text_file = file_load($form_state['values']['text']);
    $text_path = drupal_realpath($text_file->uri);
    $ds->setContentFromFile($text_path, FALSE);
    $ds->label = $text_file->filename;
    $ds->mimetype = $text_file->filemime;
  }
  $tn_file = file_load($form_state['values']['tn']);
  if (isset($tn_file->uri)) {
    $tn_path = drupal_realpath($tn_file->uri);
    $imagick = new Imagick($tn_path);
    $imagick->thumbnailimage(150, NULL);
    $imagick->setimagefilename($tn_path);
    $imagick->writeimage();

    if (empty($object['TN'])) {
      $ds = $object->constructDatastream('TN', 'M');
      $object->ingestDatastream($ds);
    }
    else {
      $ds = $object['TN'];
    }
    $ds->setContentFromFile($tn_path, FALSE);
    $ds->label = $tn_file->filename;
    $ds->mimetype = $tn_file->filemime;
  }
}
