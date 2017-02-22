(function ($) {

  Drupal.behaviors.media_fotoweb_selection_listener = {
    attach: function (context, settings) {
      function listener(event) {
        var url = 'https://fotoweb.nsf.no';
        if (event.origin != url) {
          return;
        }
        var data = event.data.asset;
        if (event.data.event === 'assetSelected') {
          console.log(data);
        }
      }

      if (window.addEventListener) {
        addEventListener('message', listener, false);
      }
      else {
        attachEvent('onmessage', listener);
      }
    }
  };

})(jQuery);
