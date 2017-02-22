<?php

class FotowebCollection implements FotowebRepresentationInterface, FotowebCollectionRepresentationInterface {

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

  public function isEmpty() {
    return empty($response['data']);
  }

  public function prev() {
    return $this->getPagingResource('prev');
  }

  public function next() {
    return $this->getPagingResource('next');
  }

  public function count() {
    return count($this->response['data']);
  }

  protected function getPagingResource($type) {
    return !empty($this->response['paging'][$type]) ? $this->response['paging'][$type] : NULL;
  }

}
