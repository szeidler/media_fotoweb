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
            dataType: '',
            success: function (data) {
              $('#ajax-result').html(data);
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
