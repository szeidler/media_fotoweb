<?php

DEFINE('FOTOWEB_PUBLIC_ENTRY_POINT', '/fotoweb');
DEFINE('FOTOWEB_PRIVATE_ENTRY_POINT', '/fotoweb/me/');

require_once 'FotowebBase.inc';
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
  protected $fullApiKey;

  public function __construct($server = NULL, $fullApiKey = NULL) {
    if (empty($server)) {
      $this->server = variable_get('media_fotoweb_server', '');
    }
    else {
      $this->server = $server;
    }

    if (empty($fullApiKey)) {
      $this->fullApiKey = variable_get('media_fotoweb_full_api_key');
    }
    else {
      $this->fullApiKey = $fullApiKey;
    }

    // For now we only support the private entry point.
    // @see: https://learn.fotoware.com/02_FotoWeb_8.0/Developing_with_the_FotoWeb_API/01_The_FotoWeb_RESTful_API/04_API_Entry_Points
    $this->entryPoint = FOTOWEB_PRIVATE_ENTRY_POINT;

    // Initialize Guzzle
    $base_uri = $this->server;
    $guzzle_configuration = array(
      'base_uri' => $base_uri,
      'allow_redirects' => FALSE,
      'headers' => ['FWAPITOKEN' => $this->getApiToken()],
    );
    drupal_alter('media_fotoweb_guzzle_configuration', $guzzle_configuration);
    $this->client = new GuzzleHttp\Client($guzzle_configuration);
  }

  public function authenticate() {
    $response = $this->initiateAuthenticationRequest();
    if ($response->getStatusCode() !== 200) {
      throw new Exception('Authentication was not successful.', 403);
    }
    return $response;
  }

  public function setFullApiKey($fullApiKey) {
    $this->fullApiKey = $fullApiKey;
  }

  public function getApiToken() {
    return $this->fullApiKey;
  }

  public function initiateAuthenticationRequest() {
    $request = new GuzzleHttp\Psr7\Request('GET', '/fotoweb/me', [
      'Accept' => 'application/vnd.fotoware.full-api-descriptor+json',
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

}