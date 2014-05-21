<?php
/**
 * @file
 * Handles the creation/display of islandora:markupEditorCModel objects.
 */

/**
 * Return an objects datastreams xml processing instructions.
 *
 * @param AbstractObject $object
 *   The Fedora object to act on.
 * @param string $dsid
 *   The datastream ID to use as source content. Defaults to OBJ.
 *
 * @return boolean|array
 *   An indexed array of processing instuctions, or FALSE if not found.
 */
function islandora_markup_editor_fetch_tei_processing_instruction(AbstractObject $object, $dsid = 'OBJ') {

  $xml_data = $object[$dsid]->content;
  $src_doc = new DOMDocument();
  $src_doc->preserveWhiteSpace = FALSE;
  $src_doc->formatOutput = TRUE;
  @$src_doc->loadXML($xml_data);

  $xpath = new DOMXPath($src_doc);
  $e = $xpath->query('//processing-instruction(\'xml-model\')');

  if (!$e->item(0)) {
    return FALSE;
  }

  // Format the output for convience.
  $arr = explode(" ", $e->item(0)->data);
  $return = array();
  foreach ($arr as $key => $value) {
    $split_arr = explode("=", $value);
    $return[$split_arr[0]] = str_replace('"', '', $split_arr[1]);
  }
  return $return;
}

/**
 * Adds schema processing instructions to a given objects requested datastream.
 *
 * @param AbstractObject $object
 *   The fedora object to act upon.
 * @param string $schema
 *   The accessable url to a relaxng .rng schema
 * @param string $dsid
 *   The dsid to add instructions to. Defaults to OBJ.
 *
 * @return string
 *   The processing instruction as it appears in the given datastream.
 */
function islandora_markup_editor_add_tei_processing_instruction(AbstractObject $object, $schema, $dsid = 'OBJ') {
  if ($schema == "") {
    global $base_url;
    $critical_edition_module_path = drupal_get_path('module', 'islandora_critical_edition');
    // Use the CWRC-TEIBasic.rng provided with the cwrc-writer by default.
    $schema = url("$base_url/$critical_edition_module_path/CWRC-Writer/src/schema/CWRC-TEIBasic.rng");
  }
  $xml_data = $object[$dsid]->content;
  $src_doc = new DOMDocument();
  $src_doc->preserveWhiteSpace = FALSE;
  $src_doc->formatOutput = TRUE;
  @$src_doc->loadXML($xml_data);

  $instructions = array(
    'href' => $schema,
    'type' => 'application/xml',
    'schematypens' => 'http://relaxng.org/ns/structure/1.0',
  );

  $processing_string = "";
  foreach ($instructions as $key => $value) {
    $processing_string .= $key . '="' . $value . '" ';
  }

  $trimmed = trim($processing_string);
  $rng_schema = $src_doc->createProcessingInstruction('xml-model', $trimmed);
  $src_doc->appendChild($rng_schema);
  $object['OBJ']->setContentFromString($src_doc->saveXML());

  return $trimmed;
}
