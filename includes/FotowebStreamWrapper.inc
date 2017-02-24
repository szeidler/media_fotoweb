<?php

/**
 *  Create an instance like this:
 *  $fotoweb = new FotowebStreamWrapper('fotoweb://a/[assetHref]');
 */
class FotowebStreamWrapper extends MediaReadOnlyStreamWrapper {

  static function getMimeType($uri, $mapping = NULL) {
    return 'image/fotoweb';
  }

}
