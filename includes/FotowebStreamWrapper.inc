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
    // Prepend exploded slash to keep Fotoweb resourceUrl intact.
    if (substr($parts[1], 0, 1) !== '/') {
      $parts[1] = '/' . $parts[1];
    }
    $params = array($parts[0] => $parts[1]);
    return $params;
  }

  /**
   * Use a hashed filename to avoid filename collisions of flattended folder
   * structure.
   *
   * @param $resourceUrl
   *
   * @return string
   */
  function getLocalFileName($resourceUrl) {
    $filename = hash('sha256', $resourceUrl);
    return $filename;
  }

  function getImagePath() {
    $parameters = $this->get_parameters();
    $local_path = file_default_scheme() . '://media-fotoweb/images/' . $this->getLocalFileName($parameters['a']) . '.jpg';

    if (!file_exists($local_path)) {
      try {
        $dirname = drupal_dirname($local_path);
        file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);

        $fotoweb = new FotowebBase();
        $fotowebAsset = new FotowebAsset($fotoweb);
        $image = $fotowebAsset->getAssetImageFromResource($parameters['a']);
        if (!empty($image)) {
          file_unmanaged_save_data($image, $local_path, FILE_EXISTS_REPLACE);
        }
      } catch (Exception $e) {
        // TODO proper exception handling.
      }
    }

    return $local_path;
  }

}
