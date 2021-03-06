/**
 * Our implementation of the CWRC-Writer
 * Delegator class. 
 * @param config
 * @returns {___anonymous83_84}
 */
function islandoraBackendDelegate(config) {
  this.writer = config.writer;
  /**
   * @param params
   * @param callback
   */
  this.lookupEntity = function(params, callback) {
    var type = params.type;
    var query = params.query;
    var lookupService = params.lookupService;
    if (lookupService == 'project') {
      var results = lookup_entity(params.type + '?entities_query=' + query);
      callback.call(this.writer, results);
    } else if (lookupService == 'viaf') {
      $.ajax({
        url: 'http://viaf.org/viaf/AutoSuggest',
        data: {
          query: query
        },
        dataType: 'jsonp',
        success: function(data, status, xhr) {
          if (data != null && data.result != null) {
            callback.call(writer, data.result);
          } else {
            callback.call(writer, []);
          }
        },
        error: function() {
          callback.call(writer, null);
        }
      });
    }
  };

  /**
   * Validates a document based on the document schema.
   * @param callback function.
   */
  this.validate = function(callback) {
    var docText = writer.fm.getDocumentContent(false);
    var usr_schema = writer.schemas[writer.schemaId];
    $.ajax({
      url: Drupal.settings.islandora_critical_edition.validate_path,
      type: 'POST',
      dataType: 'XML',
      data: {
        sch: usr_schema.url,
        type: 'RNG_XML',
        content: docText
      },
      success: function(data, status, xhr) {
        if (callback) {
          var valid = $('status', data).text() == 'pass';
          callback.call(writer, valid);
        } else {
          writer.validation.showValidationResult(data, docText);
        }
      },
      error: function() {
        writer.dialogs.show('message', {
          title: 'Error',
          msg: 'An error occurred while trying to validate the document.',
          type: 'error'
        });
      }
    });
  };

  /**
   * Loads a document based on the currentDocId
   * TODO Move currentDocId system out of CWRCWriter
   * @param docName
   */
  this.loadDocument = function(callback) {
    // So, lets enable read only mode conditionally.
    if (!Drupal.settings.islandora_markup_editor.can_edit) {
      writer.layout.hide("north");
      // Hide the tinyMCE controll bar.
      $('#editor_toolbargroup').hide();
    }
    $.ajax({
      url: Drupal.settings.basePath + 'islandora/object/' + PID + '/datastream/' + dsid + '/view',
      type: 'GET',
      async: false,
      dataType: 'xml',
      success: function(doc, status, xhr) {
        window.location.hash = '#' + PID;
        callback.call(writer, doc);
      },
      error: function(xhr, status, error) {
        writer.dialogs.show('message', {
          title: 'Error',
          msg: 'An error (' + status + ') occurred and ' + PID + ' was not loaded.',
          type: 'error'
        }); 
        writer.currentDocId = null;
      }
    });
  };
  /**
   * Performs the server call to save the document.
   * @param callback Called with one boolean parameter: true for successful save, false otherwise
   */
  this.saveDocument = function(callback) {
    if (!Drupal.settings.islandora_markup_editor.can_edit) {
      writer.dialogs.show('message', {
        title: 'Please Authenticate',
        msg: 'You do not possess the sufficient permissions to edit this data.',
        type: 'error'
      });
      return;
    }
    
    writer.mode == writer.XMLRDF;
    var docText = writer.fm.getDocumentContent(true);
    var usr_schema = writer.schemas[writer.schemaId];
    $.ajax({
      url: window.parent.Drupal.settings.basePath + 'islandora/markupeditor/save_data/' + PID,
      type: 'POST', 
      async: false,
      dataType: 'text',
      data: {
        "text": docText,
        "valid": true,
        "schema": usr_schema.url,
      },
      success: function(data, status, xhr) {
    	  console.log("save success");
        writer.editor.isNotDirty = 1; // force clean state
        writer.dialogs.show('message', {
          title: 'Document Saved',
          msg: PID + ' was saved successfully.'
        });
        window.location.hash = '#' + PID;
        if (callback) {
          callback.call(writer, true);
        }
      },
      error: function() {
        writer.dialogs.show('message', {
          title: 'Error',
          msg: 'An error occurred and ' + PID + ' was not saved.',
          type: 'error'
        });
        if (callback) {
          callback.call(writer, false);
        }
      }
    });
  };

  this.getHelp = function(tagName) {
    return this.writer.u.getDocumentationForTag(tagName);
  };

  this.editorCallback = function(name, data) {
    switch (name) {
      case 'highlightEntity_looseFocus':
        if ($(data).hasClass('txtimglnk')) {
          //islandoraCWRCWriter.Writer.Extensions.text_image_linking_hide_highlight(data);
        }
        break;
      case 'highlightEntity_gainFocus':
        if ($(data).hasClass('txtimglnk')) {
          //islandoraCWRCWriter.Writer.Extensions.text_image_linking_show_highlight(data);
        }
        break;
      case 'editor_settingsChanged' :
        var docText = writer.getDocument();
        writer.schemaId = data['schemaId'];
        writer.loadDocument(docText);
        break;
    }
  };
}
