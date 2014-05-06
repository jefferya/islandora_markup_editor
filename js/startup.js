var dsid = "OBJ";
$(function() {
  // Hide the content we dont need.
  $('#page_selector').hide();
  $('#header-inner').hide();
  $('#pageChange').hide();
  $('#header_label_wrapper').hide();
  $('#annotation_tab').hide();

  
  // This will load another Datastream into the viewer.
//  $('#change-ds-button').click(function() {
//    dsid = 'TST_BKL';
//    $.ajax({
//      url: Drupal.settings.basePath + 'islandora/object/' + PID + '/datastream/' + dsid + '/view',
//      type: 'GET',
//      async: false,
//      dataType: 'xml',
//      success: function(doc, status, xhr) {
//        window.location.hash = '#' + PID;
//        writer.fm.loadDocumentFromXml(doc);
//      },
//      error: function(xhr, status, error) {
//        writer.dialogs.show('message', {
//          title: 'Error',
//          msg: 'An error (' + status + ') occurred and ' + PID + ' was not loaded.',
//          type: 'error'
//        });
//        writer.currentDocId = null;
//      }
//    });
//  });
  
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
  console.log(Drupal.settings.islandora_critical_edition.schema_object['schemas']);
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
      writer.schemaId = "tei";
      writer.init();
      // Close the UIPanes.
      writer.layout.hide("east");
      writer.layout.toggle("west");
      
//      console.log($('#editor_toolbargroup'));
//      $('#editor_toolbargroup').hide();
      
    },
    error: function() {
      console.log("Error");
    }
  });
});