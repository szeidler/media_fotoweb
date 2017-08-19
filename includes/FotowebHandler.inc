<?php

/**
 * FotowebHandler acts as a bridge between the StreamWrapper and file objects.
 */
class FotowebHandler {

  protected $fotowebResource;
  protected $isValid;

  /**
   * FotowebHandler constructor.
   *
   * @param string $fotowebResource
   *   Fotoweb resource as a URI.
   * @param bool $isValid
   *   Information about the validity and existing state of a Fotoweb asset.
   */
  public function __construct($fotowebResource, $isValid = FALSE) {
    $this->fotowebResource = $fotowebResource;
    $this->isValid = $isValid;
  }

  /**
   * Parse Fotoweb resource to a normalized file stream.
   *
   * @param string $fotowebResource
   *   Fotoweb resource as a URI.
   *
   * @return string
   *   The normalized file uri.
   */
  public function parse($fotowebResource) {
    $components = parse_url($fotowebResource);
    if (!empty($components['path'])) {
      $path = $components['path'];
    }
    else {
      $path = $fotowebResource;
    }

    // TODO check if it is a valid fotoweb resource.
    if (!$this->isValid) {

    }
    return file_stream_wrapper_uri_normalize('fotoweb://a' . $path);
  }

  /**
   * Creates a file object, based on the Fotoweb resource.
   *
   * @return bool|object
   *   Drupal file object.
   */
  public function getFileObject() {
    $uri = $this->parse($this->fotowebResource);
    $file = file_uri_to_object($uri, TRUE);
    return $file;
  }

}
