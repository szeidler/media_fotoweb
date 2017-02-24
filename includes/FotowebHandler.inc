<?php

class FotowebHandler {

  protected $fotowebResource;
  protected $isValid;

  public function __construct($fotowebResource, $isValid = FALSE) {
    $this->fotowebResource = $fotowebResource;
    $this->isValid = $isValid;
  }

  /**
   * Parse Fotoweb resource to a normalized file stream.
   *
   * @param $fotowebResource
   * @param bool $isValid
   *   Option to skip resource validation. Use when processing a resource
   *   coming from a Fotoweb response.
   *
   * @return string
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

  public function getFileObject() {
    $uri = $this->parse($this->fotowebResource);
    $file = file_uri_to_object($uri, TRUE);
    return $file;
  }
}
