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

  public function getDirectoryPath() {
    return variable_get('file_public_path', conf_path() . '/files') . '/media-fotoweb';
  }

  /**
   * Return local file for realpath(), because modules like ImageMagick expect
   * realpath() to return a valid file resource, for retrieving filesize().
   */
  public function realpath() {
    return drupal_realpath($this->getLocalImagePath());
  }

  /**
   * @inheritdoc
   */
  public function getExternalUrl() {
    // If we have stored a current image style, return its derivative.
    if (!empty($this->current_image_style)) {
      $derivative_uri = image_style_path($this->current_image_style, $this->getUri());
      $derivative_uri = str_replace('public:/', '', $derivative_uri);
      return url($this->getDirectoryPath() . $derivative_uri, array('absolute' => TRUE));
    }
    if ($parameters = $this->get_parameters()) {
      if (isset($parameters['a'])) {
        return file_create_url($this->getLocalImagePath());
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
   * Use a hashed filename to avoid filename collisions of flattened folder
   * structure.
   *
   * @param $resourceUrl
   *
   * @return string
   */
  public function getLocalFileName($resourceUrl) {
    $uri = 'fotoweb://a' . $resourceUrl;
    $filename = hash('sha256', $uri);
    return $filename;
  }

  /**
   * Returns the local image path and trigger file save, when local file does
   * not exist.
   *
   * @return string
   */
  public function getLocalImageUri() {
    $local_path = $this->getLocalImagePath();

    if (!file_exists($local_path)) {
      $dirname = drupal_dirname($local_path);
      file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);

      // Return the original image url based on the sites storage type selection.
      $file_storage_type = variable_get('media_fotoweb_file_storage_type', 'original');
      switch ($file_storage_type) {
        case 'preview':
          $url = $this->getPreviewImageUrl();
          $fotoweb = new FotowebBase();
          break;
        default:
          $fotoweb = new FotowebArchiveAgentBase();
          // ArchiveAgent requests need to authenticate with the current user.
          module_load_include('inc', 'media_fotoweb', '/includes/media_fotoweb.helper');
          $fotoweb->setUsername(_media_fotoweb_get_fotoweb_username());
          $url = $this->getOriginalImageUrl();
      }

      $response = $fotoweb->initiateRequest($url);

      if ($response && $response->getStatusCode() == 200) {
        file_unmanaged_save_data($response->getBody(), $local_path, FILE_EXISTS_REPLACE);
      }
    }

    return $local_path;
  }

  /**
   * Returns the local image path for the fotoweb asset.
   *
   * @return string
   */
  public function getLocalImagePath() {
    $parameters = $this->get_parameters();
    $local_path = file_default_scheme() . '://media-fotoweb/images/' . $this->getLocalFileName($parameters['a']) . '.jpg';
    return $local_path;
  }

  /**
   * Returns the original image url from the fotoweb asset resource.
   *
   * @return mixed|null|\Psr\Http\Message\ResponseInterface
   */
  public function getOriginalImageUrl() {
    $parts = $this->get_parameters();
    $resourceUrl = check_plain($parts['a']);

    $fotoweb = new FotowebBase();
    $fotowebArchiveAgent = new FotowebArchiveAgentBase();
    // ArchiveAgent requests need to authenticate with the current user.
    module_load_include('inc', 'media_fotoweb', '/includes/media_fotoweb.helper');
    $fotowebArchiveAgent->setUsername(_media_fotoweb_get_fotoweb_username());

    $fotowebAsset = new FotowebAsset($fotoweb);
    $fotowebArchiveAgentOriginalImage = new FotowebArchiveAgentOriginalImage($fotowebArchiveAgent, $fotowebAsset);
    try {
      $url = $fotowebArchiveAgentOriginalImage->getOriginalImageDownloadUrlFromResource($resourceUrl);
    } catch (Exception $e) {
      watchdog('media_fotoweb', $e->getMessage(), array(), WATCHDOG_CRITICAL);
      return NULL;
    }
    return $url;
  }

  /**
   * Returns the preview image url from the fotoweb asset resource.
   *
   * @return mixed|null|\Psr\Http\Message\ResponseInterface
   */
  public function getPreviewImageUrl() {
    $parts = $this->get_parameters();
    $resourceUrl = check_plain($parts['a']);

    $fotoweb = new FotowebBase();
    $localFileThreshold = variable_get('media_fotoweb_local_file_size_threshold', 1500);
    $fotowebAsset = new FotowebAsset($fotoweb, $localFileThreshold);
    try {
      $path = $fotowebAsset->getAssetImagePathFromResource($resourceUrl);
      $url = $fotoweb->getServer() . $path;
    } catch (Exception $e) {
      watchdog('media_fotoweb', $e->getMessage(), array(), WATCHDOG_CRITICAL);
      return NULL;
    }
    return $url;
  }

}

