<?php

/**
 * Class FotowebAsset
 *
 * @see https://learn.fotoware.com/02_FotoWeb_8.0/Developing_with_the_FotoWeb_API/01_The_FotoWeb_RESTful_API/Asset_representation
 */
class FotowebAsset implements FotowebRepresentationInterface {

  protected $fotoweb;

  const headerAccept = 'application/vnd.fotoware.asset+json';

  public function __construct(FotowebBase $fotoweb) {
    $this->fotoweb = $fotoweb;
  }

  public function initiateRequest($resourceUrl) {
    $this->fotoweb->ensureAuthentication();

    $url = $this->fotoweb->resourceUrlToUrl($resourceUrl);
    $options = array(
      'method' => 'GET',
      'headers' => array(
        'Accept' => self::headerAccept,
        'FWAPITOKEN' => $this->getNegotiatedToken(),
      ),
    );

    return $this->fotoweb->request($url, $options);
  }

  public function getAssetThumbnail($resourceUrl) {
    $response = $this->initiateRequest($resourceUrl);
  }
}
