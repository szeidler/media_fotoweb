(function (Drupal, drupalSettings) {

  Drupal.behaviors.mediaFotowebSelectionListener = {
    attach: function (context, settings) {
      function fotowebSelectionlistener(event) {
        var url = drupalSettings.media_fotoweb.host;

        // Listen for messages only from the Fotoweb API.
        if (event.origin != url) {
          return;
        }

        // Post the retrieved asset data with an AJAX request.
        var asset = event.data.asset
        if (event.data.event === 'assetSelected') {
          var asset_json = JSON.stringify([asset]);
          var selection_storage_fields = document.getElementsByName('fotoweb_selected');
          for (var i = 0; i < selection_storage_fields.length; i++) {
            selection_storage_fields[i].value = asset_json;
          }
          var entity_browser_form_submit = document.querySelector('form.entity-browser-form #edit-submit');
          if (entity_browser_form_submit) {
            entity_browser_form_submit.click();
          }
          else {
            console.error('No entity browser form found. Fotoweb asset could not be imported.')
          }
        }
      }

      if (window.addEventListener) {
        addEventListener('message', fotowebSelectionlistener, false);
      } else {
        attachEvent('onmessage', fotowebSelectionlistener);
      }
    }
  };

})(Drupal, drupalSettings);
