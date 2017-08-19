<?php

// @codingStandardsIgnoreFile

/**
 * FotowebStreamWrapper is a custom StreamWrapper for Fotoweb assets.
 *
 * Create an instance like this:
 *  $fotoweb = new FotowebStreamWrapper('fotoweb://a/[assetHref]');
 */
class FotowebStreamWrapper extends MediaReadOnlyStreamWrapper {

  protected $base_url;
  protected $current_image_style = NULL;
  protected $stream_content = NULL;
  protected $stream_pointer;


  /**
   * FotowebStreamWrapper constructor.
   */
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
   *   True, if it is a image style request.
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
    // Fotoweb uses relative paths as identifier, so we need to split
    // only the first parameter.
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
   * @param string $resourceUrl
   *   Fotoweb resource as a URI.
   *
   * @return string
   *   Filename used to store the Fotoweb asset.
   */
  public function getLocalFileName($resourceUrl) {
    $uri = 'fotoweb://a' . $resourceUrl;
    $filename = hash('sha256', $uri);
    return $filename;
  }

  /**
   * Returns the local image path.
   *
   * Triggers also file save, when local file does not already exist.
   *
   * @return string
   *   Full local URI to store the Fotoweb asset locally.
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

      // Request and store the fotoweb image.
      // If it fails return NULL.
      if (!$this->storeImage($fotoweb, $url, $local_path)) {
        return NULL;
      }
    }

    return $local_path;
  }

  /**
   * Fetches the fotoweb image and store it on the disk.
   *
   * @param $fotoweb
   *   Fotoweb Handler.
   * @param string $url
   *   Remote URL of the fotoweb image.
   * @param $local_path
   *   Local path, where the image should be stored.
   *
   * @return bool
   *   Status of the process was successful.
   */
  protected function storeImage($fotoweb, $url, $local_path) {
    if (empty($url)) return FALSE;

    $response = $fotoweb->initiateRequest($url);

    if ($response && $response->getStatusCode() == 200) {
      $this->stream_content = (string) $response->getBody();
      if (!file_unmanaged_save_data($response->getBody(), $local_path, FILE_EXISTS_REPLACE)) {
        return FALSE;
      }
    }
  }

  /**
   * Returns the local image path for the fotoweb asset.
   *
   * @return string
   *   Complete local image path to store the asset.
   */
  public function getLocalImagePath() {
    $parameters = $this->get_parameters();
    $local_path = file_default_scheme() . '://media-fotoweb/images/' . $this->getLocalFileName($parameters['a']) . '.jpg';
    return $local_path;
  }

  /**
   * Returns the original image url from the fotoweb asset resource.
   *
   * @return string|null
   *   URL the original image can be fetched from.
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
   * @return string|null
   *   URL the preview image can be fetched from.
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

  /**
   * Write back asset metadata for the current Fotoweb asset.
   *
   * @param $metadata
   *   Asset metadata properties to be updated.
   *
   * @return bool
   *   TRUE, if the metadata was successfully updated. FALSE if not.
   */
  public function update_metadata($metadata) {
    $parts = $this->get_parameters();
    $resourceUrl = check_plain($parts['a']);

    $fotoweb = new FotowebBase();
    $fotowebAsset = new FotowebAsset($fotoweb);

    try {
      return $fotowebAsset->updateMetadata($resourceUrl, $metadata);
    } catch (Exception $e) {
      watchdog('media_fotoweb', $e->getMessage(), array(), WATCHDOG_ERROR);
      return FALSE;
    }
  }

  /**
   * Support for fopen(), file_get_contents(), file_put_contents() etc.
   *
   * @param $uri
   *   A string containing the URI to the file to open.
   * @param $mode
   *   The file mode ("r", "wb" etc.).
   * @param $options
   *   A bit mask of STREAM_USE_PATH and STREAM_REPORT_ERRORS.
   * @param &$opened_path
   *   A string containing the path actually opened.
   *
   * @return
   *   Returns TRUE if file was opened successfully.
   *
   * @see http://php.net/manual/en/streamwrapper.stream-open.php
   */
  public function stream_open($uri, $mode, $options, &$opened_path) {
    $this->setUri($uri);

    $allowed_modes = array('r', 'rb');
    if (!in_array($mode, $allowed_modes)) {
      return FALSE;
    }

    // Attempt to fetch the URL's data.
    if (!$this->getStreamContent()) {
      return FALSE;
    }

    // Reset the stream pointer since this is an open.
    $this->stream_pointer = 0;
    return TRUE;
  }

  /**
   * Support for flock().
   *
   * @param $operation
   *   One of the following:
   *   - LOCK_SH to acquire a shared lock (reader).
   *   - LOCK_EX to acquire an exclusive lock (writer).
   *   - LOCK_UN to release a lock (shared or exclusive).
   *   - LOCK_NB if you don't want flock() to block while locking (not
   *     supported on Windows).
   *
   * @return
   *   Always returns TRUE at the present time.
   *
   * @see http://php.net/manual/en/streamwrapper.stream-lock.php
   */
  public function stream_lock($operation) {
    return TRUE;
  }

  /**
   * Support for fread(), file_get_contents() etc.
   *
   * @param $count
   *   Maximum number of bytes to be read.
   *
   * @return
   *   The string that was read, or FALSE in case of an error.
   *
   * @see http://php.net/manual/en/streamwrapper.stream-read.php
   */
  public function stream_read($count) {
    if (is_string($this->stream_content)) {
      $remaining_chars = strlen($this->stream_content) - $this->stream_pointer;
      $number_to_read = min($count, $remaining_chars);
      if ($remaining_chars > 0) {
        $buffer = substr($this->stream_content, $this->stream_pointer, $number_to_read);
        $this->stream_pointer += $number_to_read;
        return $buffer;
      }
    }
    return FALSE;
  }

  /**
   * Support for fwrite(), file_put_contents() etc.
   *
   * @param $data
   *   The string to be written.
   *
   * @return
   *   The number of bytes written (integer).
   *
   * @see http://php.net/manual/en/streamwrapper.stream-write.php
   */
  public function stream_write($data) {
    return FALSE;
  }

  /**
   * Support for feof().
   *
   * @return
   *   TRUE if end-of-file has been reached.
   *
   * @see http://php.net/manual/en/streamwrapper.stream-eof.php
   */
  public function stream_eof() {
    return $this->stream_pointer == strlen($this->stream_content);
  }

  /**
   * Support for fseek().
   *
   * @param $offset
   *   The byte offset to got to.
   * @param $whence
   *   SEEK_SET, SEEK_CUR, or SEEK_END.
   *
   * @return
   *   TRUE on success.
   *
   * @see http://php.net/manual/en/streamwrapper.stream-seek.php
   */
  public function stream_seek($offset, $whence) {
    if (strlen($this->stream_content) >= $offset) {
      $this->stream_pointer = $offset;
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Support for fflush().
   *
   * @return
   *   TRUE if data was successfully stored (or there was no data to store).
   *
   * @see http://php.net/manual/en/streamwrapper.stream-flush.php
   */
  public function stream_flush() {
    return TRUE;
  }

  /**
   * Support for ftell().
   *
   * @return
   *   The current offset in bytes from the beginning of file.
   *
   * @see http://php.net/manual/en/streamwrapper.stream-tell.php
   */
  public function stream_tell() {
    return $this->stream_pointer;
  }

  /**
   * Support for fstat().
   *
   * @return
   *   An array with file status, or FALSE in case of an error - see fstat()
   *   for a description of this array.
   *
   * @see http://php.net/manual/en/streamwrapper.stream-stat.php
   */
  public function stream_stat() {
    $stat = array();
    $this->parameters = $this->_parse_url($this->getUri());
    if ($size = strlen($this->getStreamContent())) {
      $stat['size'] = $size;
    }

    return !empty($stat) ? $this->getStat($stat) : FALSE;
  }

  /**
   * Support for fclose().
   *
   * @return
   *   TRUE if stream was successfully closed.
   *
   * @see http://php.net/manual/en/streamwrapper.stream-close.php
   */
  public function stream_close() {
    $this->stream_pointer = 0;
    $this->stream_content = NULL;
    return TRUE;
  }

  /**
   * Support for stat().
   *
   * @param $uri
   *   A string containing the URI to get information about.
   * @param $flags
   *   A bit mask of STREAM_URL_STAT_LINK and STREAM_URL_STAT_QUIET.
   *
   * @return
   *   An array with file status, or FALSE in case of an error - see fstat()
   *   for a description of this array.
   *
   * @see http://php.net/manual/en/streamwrapper.url-stat.php
   */
  public function url_stat($uri, $flags) {
    $this->uri = $uri;
    if ($flags & STREAM_URL_STAT_QUIET) {
      return @$this->stream_stat();
    }
    else {
      return $this->stream_stat();
    }
  }

  /**
   * Helper function to return a full array for stat functions.
   *
   * @param array $stat
   *   State array to be merged with the default values.
   *
   * @return array
   *   Stat array.
   */
  protected function getStat(array $stat = array()) {
    $defaults = $this->_stat;

    $return = array();
    foreach (array_keys($defaults) as $index => $key) {
      if (!isset($stat[$key])) {
        $return[$index] = $defaults[$key];
        $return[$key] = $defaults[$key];
      }
      else {
        $return[$index] = $stat[$key];
        $return[$key] = $stat[$key];
      }
    }

    return $return;
  }

  /**
   * Fetch the content of the file loading the local file.
   *
   * If the local file not yet exists getLocalImageUri() will take care of
   * its creation.
   *
   * @return bool|null|string
   */
  protected function getStreamContent() {
    if (!isset($this->stream_content)) {
      $this->stream_content = NULL;
      // $this->getLocalImageUri() sets the stream_content on new file_import,
      // or we can fetch it from the local file afterwards.
      if (($localPath = $this->getLocalImageUri()) && empty($this->stream_content)) {
        $this->stream_content = file_get_contents($localPath);
      }
    }

    return $this->stream_content;
  }

}

