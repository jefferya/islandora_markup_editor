<?php
/**
 * @file
 * Handles the creation/display of islandora:markupEditorCModel objects.
 */

function islandora_markup_editor_fetch_tei_processing_instruction(AbstractObject $object, $dsid = 'OBJ') {

  $xml_data = $object[$dsid]->content;
  $srcDoc = new DOMDocument;
  $srcDoc->preserveWhiteSpace = FALSE;
  $srcDoc->formatOutput = TRUE;
  @$srcDoc->loadXML($xml_data);

  $xpath = new DOMXPath($srcDoc);
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

function islandora_markup_editor_add_tei_processing_instruction(AbstractObject $object, $schema) {
  if ($schema == "") {
    // Default Schema.
    $schema = 'http://cwrc.ca/schema/workflow.rng';
  }
  dsm($schema, "setting schema to: ");
  $xml_data = $object['OBJ']->content;
  $srcDoc = new DOMDocument;
  $srcDoc->preserveWhiteSpace = false;
  $srcDoc->formatOutput = true;
  @$srcDoc->loadXML( $xml_data );

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

  dsm($trimmed, "trimmed");
  $rng_schema = $srcDoc->createProcessingInstruction('xml-model', $trimmed);
  $srcDoc->appendChild($rng_schema);
  $object['OBJ']->setContentFromString($srcDoc->saveXML());

  return $trimmed;
}