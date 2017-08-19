<?php

use GuzzleHttp\Psr7\Request;

/**
 * FotowebCollection handles all interactions with a Fotoweb collection.
 */
class FotowebCollection implements FotowebRepresentationInterface {

  protected $fotoweb;

  const HEADER_ACCEPT = 'application/vnd.fotoware.collection+json';

  /**
   * FotowebCollection constructor.
   *
   * @param FotowebBase $fotoweb
   *   Fotoweb Base object.
   */
  public function __construct(FotowebBase $fotoweb) {
    $this->fotoweb = $fotoweb;
  }

  /**
   * Initiates a HTTP request using Guzzle.
   *
   * @inheritdoc
   */
  public function initiateRequest($resourceUrl) {
    $request = new Request('GET', $resourceUrl, [
      'Accept' => static::HEADER_ACCEPT,
    ]);

    return $this->fotoweb->initiateRequest($request);
  }

}
