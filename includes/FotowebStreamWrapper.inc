<?php

/**
 *  Create an instance like this:
 *  $fotoweb = new FotowebStreamWrapper('fotoweb://a/[assetHref]');
 */
class FotowebStreamWrapper extends MediaReadOnlyStreamWrapper {

  protected $base_url;
  protected $current_image_style = NULL;

  public function __construct() {
    $this->base_url = variable_get('media_fotoweb_server', '');
  }


  /**
   * @inheritdoc
   */
  static function getMimeType($uri, $mapping = NULL) {
    return 'image/fotoweb';
  }

  /**
   * @inheritdoc
   */
  public function interpolateUrl() {
    // If we have stored a current image style, return its derivative.
    if (!empty($this->current_image_style)) {
      $derivative_uri = image_style_path($this->current_image_style, $this->getUri());
      // Force the image derivative to be created, because remote stream wrappers, are not returning valid tokens.
      // @see https://www.drupal.org/node/2791283
      image_style_create_derivative(image_style_load($this->current_image_style), $this->getUri(), $derivative_uri);
      return file_create_url($derivative_uri);
    }
    if ($parameters = $this->get_parameters()) {
      if (isset($parameters['a'])) {
        return $this->base_url . $parameters['a'];
      }
    }
  }

  /**
   * @inheritdoc
   */
  public function setUri($uri) {
    $this->uri = $uri;
    $this->parameters = $this->_parse_url($uri);

    if ($this->isImageStyle()) {
      $this->uri = $this->getLocalImagePath();
    }
  }

  /**
   * Checks, if the current file uri is an image style resource.
   *
   * @return bool
   */
  protected function isImageStyle() {
    if (($target = file_uri_target($this->getUri())) && strpos($target, 'styles/') === 0) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  /**
   * @inheritdoc
   */
  protected function _parse_url($url) {
    // Fotoweb uses relative paths as identifier, so we need to split only the first parameter.
    $path = explode('://', $url);
    $parts = explode('/', $path[1]);
    if ($this->isImageStyle()) {
      $this->current_image_style = $parts[1];
      unset($parts[0]);
      unset($parts[1]);
      unset($parts[2]);
      if ($parts[3] === 'a') {
        unset($parts[3]);
        $params = array('a' => '/' . implode('/', $parts));
      }
    }
    else {
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
    }
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
  protected function getLocalFileName($resourceUrl) {
    $filename = hash('sha256', $resourceUrl);
    return $filename;
  }

  /**
   * Returns the local image path and trigger file save, when local file does
   * not exist.
   *
   * @return string
   */
  public function getLocalImagePath() {
    $parameters = $this->get_parameters();
    $local_path = file_default_scheme() . '://media-fotoweb/images/' . $this->getLocalFileName($parameters['a']) . '.jpg';

    if (!file_exists($local_path)) {
      $dirname = drupal_dirname($local_path);
      file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);

      $response = $this->getOriginalImagePath();
      if ($response && $response->getStatusCode() == 200) {
        file_unmanaged_save_data($response->getBody(), $local_path, FILE_EXISTS_REPLACE);
      }
    }

    return $local_path;
  }

  /**
   * Returns the original image path from the fotoweb asset resource.
   *
   * @return mixed|null|\Psr\Http\Message\ResponseInterface
   */
  protected function getOriginalImagePath() {
    $parts = $this->get_parameters();
    $uri = file_stream_wrapper_uri_normalize('fotoweb://a' . check_plain($parts['a']));
    $resourceUrl = file_create_url($uri);

    $fotoweb = new FotowebBase();
    $localFileThreshold = variable_get('media_fotoweb_local_file_max_size', 1920);
    $fotowebAsset = new FotowebAsset($fotoweb, $localFileThreshold);
    try {
      $response = $fotowebAsset->getAssetImageFromResource($resourceUrl);
    } catch (Exception $e) {
      watchdog('media_fotoweb', $e->getMessage(), array(), WATCHDOG_CRITICAL);
      return NULL;
    }
    return $response;
  }
}

