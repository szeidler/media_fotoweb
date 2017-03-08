<?php

class FotowebCollection implements FotowebRepresentationInterface {

  protected $fotoweb;

  const headerAccept = 'application/vnd.fotoware.collection+json';

  public function __construct(FotowebBase $fotoweb) {
    $this->fotoweb = $fotoweb;
  }

  public function initiateRequest($resourceUrl) {
    $this->fotoweb->ensureAuthentication();

    $request = new GuzzleHttp\Psr7\Request('GET', $resourceUrl, [
      'Accept' => static::headerAccept,
    ]);

    return $this->fotoweb->request($request);
  }

}
