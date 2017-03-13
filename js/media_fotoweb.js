(function ($) {

  Drupal.behaviors.media_fotoweb_selection_listener = {
    attach: function (context, settings) {
      function fotowebSelectionlistener(event) {
        var url = settings.media_fotoweb.fotoweb_host;

        // Listen for messages only from the Fotoweb API.
        if (event.origin != url) {
          return;
        }

        // Post the retrieved asset data with an AJAX request.
        var data = event.data.asset;
        if (event.data.event === 'assetSelected') {
          $.ajax({
            url: Drupal.settings.media_fotoweb.assetAddAjaxUrl,
            type: 'POST',
            data: {asset: data},
            dataType: 'json',
            success: function (data) {
              if ('fid' in data) {
                // Naively reloading the media browser with attached file id.
                // That will trigger the file to be further processed.
                // @see media_browser()
                window.location = Drupal.settings.basePath + Drupal.settings.pathPrefix + 'media/browser/?fid=' + data.fid;
              }
            }
          });
        }
      }

      if (window.addEventListener) {
        addEventListener('message', fotowebSelectionlistener, false);
      }
      else {
        attachEvent('onmessage', fotowebSelectionlistener);
      }
    }
  };

})(jQuery);
