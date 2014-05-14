var dsid = "OBJ";
$(function() {
  // Hide the content we dont need.
  $('#page_selector').hide();
  $('#header-inner').hide();
  $('#pageChange').hide();
  $('#header_label_wrapper').hide();
  $('#annotation_tab').hide();

  PID = Drupal.settings.islandora_critical_edition.page_pid;
  cwrc_params = {};
  window.location.hash = '#' + PID;
  writer = null;
  islandoraCriticalEditionsUrl = Drupal.settings.basePath +
    Drupal.settings.islandora_critical_edition.module_base;
  var config = {
    delegator: islandoraBackendDelegate,
    cwrcRootUrl: islandoraCriticalEditionsUrl + '/CWRC-Writer/src/',
    schemas: Drupal.settings.islandora_critical_edition.schema_object['schemas']
  };
  $.ajax({
    url: Drupal.settings.basePath + Drupal.settings.islandora_critical_edition.page_setup + PID,
    timeout: 3000,
    async: false,
    dataType: 'json',
    success: function(data, status, xhr) {
      cwrc_params = data;
      config.project = data;
      writer = new Writer(config);
      writer.currentDocId = PID;
      writer.schemaId = "doc_default";
      writer.init();
      // Close the UIPanes.
      writer.layout.hide("east");
      writer.layout.toggle("west");
      
    },
    error: function() {
      console.log("Error");
    }
  });
});