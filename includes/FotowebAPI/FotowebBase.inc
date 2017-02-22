<?php

DEFINE('FOTOWEB_PUBLIC_ENTRY_POINT', '/fotoweb');
DEFINE('FOTOWEB_PRIVATE_ENTRY_POINT', '/fotoweb/me/');

require_once 'FotowebBase.inc';
require_once 'FotowebCollectionRepresentationInterface.inc';
require_once 'FotowebRepresentationInterface.inc';
require_once 'FotowebCollection.inc';
require_once 'FotowebCollectionList.inc';
require_once 'FotowebArchives.inc';
require_once 'FotowebAssetList.inc';
require_once 'FotowebAsset.inc';

class FotowebBase {

  protected $server;
  protected $client;
  protected $entryPoint;
  protected $fullApiToken;

  public function __construct($server = NULL, $fullApiToken = NULL) {
    if (empty($server)) {
      $this->server = variable_get('media_fotoweb_server', '');
    }
    else {
      $this->server = $server;
    }

    if (empty($fullApiToken)) {
      $this->fullApiToken = variable_get('media_fotoweb_full_api_token');
    }
    else {
      $this->fullApiToken = $fullApiToken;
    }

    // For now we only support the private entry point.
    // @see: https://learn.fotoware.com/02_FotoWeb_8.0/Developing_with_the_FotoWeb_API/01_The_FotoWeb_RESTful_API/04_API_Entry_Points
    $this->entryPoint = FOTOWEB_PRIVATE_ENTRY_POINT;

    // Initialize Guzzle
    $base_uri = $this->server . $this->entryPoint;
    $this->client = new GuzzleHttp\Client(array(
      'base_uri' => $base_uri,
      'proxy' => 'socks5://10.254.254.254:8123',
      'allow_redirects' => FALSE,
      'headers' => ['FWAPITOKEN' => $this->getApiToken()],
    ));
  }

  public function authenticate() {
    $response = $this->initiateRequestByMethod('authenticate');
    return $response;
  }

  public function ensureAuthentication() {
    /*$tokenExpireTimeStamp = strtotime($_SESSION['FWUserTokenExpire']);

    // When the token is expired, reauthenticate the user.
    if ($tokenExpireTimeStamp < time()) {
      $this->authenticate();
    }*/
  }

  public function setFullApiToken($fullApiToken) {
    $this->fullApiToken = $fullApiToken;
  }

  public function getApiToken() {
    return $this->fullApiToken;
  }

  public function initiateRequestByMethod($method) {
    if ($method !== 'authenticate') {
      // Make sure, that the user is authenticated before performing each request.
      $this->ensureAuthentication();
    }

    $request = new GuzzleHttp\Psr7\Request('GET', '/fotoweb/me', [
      'Accept' => $this->methodToMediaType($method),
      'FWAPITOKEN' => $this->getApiToken(),
    ]);

    return $this->initiateRequest($request);
  }

  protected function initiateRequest($request) {
    $response = $this->request($request);
    return $response;
  }

  public function request($request) {
    $response = $this->client->send($request);
    return $response;
  }

  protected function methodToMediaType($method) {
    switch ($method) {
      case 'authenticate':
        $mediaType = 'application/vnd.fotoware.full-api-descriptor+json';
        break;
      default:
        $mediaType = 'application/vnd.fotoware.full-api-descriptor+json';
    }
    return $mediaType;
  }

  public function resourceUrlToUrl($resourceUrl) {
    $url = $this->server . $this->entryPoint . $resourceUrl;
    return $url;
  }

  protected function methodToUrl($method) {
    switch ($method) {
      case 'authenticate':
        $url = '/fotoweb/me';
        break;
      default:
        $url = $method;
    }
    return $url;
  }

}