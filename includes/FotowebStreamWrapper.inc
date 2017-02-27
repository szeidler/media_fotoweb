<?php

/**
 *  Create an instance like this:
 *  $fotoweb = new FotowebStreamWrapper('fotoweb://a/[assetHref]');
 */
class FotowebStreamWrapper extends MediaReadOnlyStreamWrapper {

  /**
   * @inheritdoc
   */
  static function getMimeType($uri, $mapping = NULL) {
    return 'image/fotoweb';
  }

  /**
   * @inheritdoc
   */
  protected function _parse_url($url) {
    // Fotoweb uses relative paths as identifier, so we need to split only the first parameter.
    $path = explode('://', $url);
    $parts = explode('/', $path[1], 2);
    $total = count($parts);
    if (!$total || ($total % 2)) {
      // If we have no parts, or an odd number of parts, it's malformed.
      return FALSE;
    }
    $params = array($parts[0] => $parts[1]);
    return $params;
  }

  function getLocalFileName($resourceUrl) {
    $parts = explode('/', check_plain($resourceUrl));
    $filename = array_pop($parts);

    // Remove .info suffix.
    $filename = str_replace('.info', '', $filename);
    return $filename;
  }

  function getThumbnailPath() {
    $parameters = $this->get_parameters();
    $local_path = file_default_scheme() . '://media-fotoweb/thumbnails/' . $this->getLocalFileName($parameters['a']);

    if (!file_exists($local_path)) {
      try {
        $dirname = drupal_dirname($local_path);
        file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);

        $fotoweb = new FotowebBase();
        $fotowebAsset = new FotowebAsset($fotoweb);
        $thumbnailImage = $fotowebAsset->getAssetThumbnailFromResource($parameters['a']);
        if (!empty($thumbnailImage)) {
          file_unmanaged_save_data($thumbnailImage, $local_path, TRUE);
        }
      } catch (Exception $e) {
        // TODO proper exception handling.
      }
    }

    return $local_path;
  }

}
