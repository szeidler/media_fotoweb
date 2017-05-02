<?php

DEFINE('FOTOWEB_PUBLIC_ENTRY_POINT', '/fotoweb');
DEFINE('FOTOWEB_PRIVATE_ENTRY_POINT', '/fotoweb/me/');

require_once 'FotowebLoginTokenGenerator.inc';

require_once 'FotowebGuzzleConfiguratorInterface.inc';
require_once 'FotowebGuzzleConfigurator.inc';
require_once 'FotowebGuzzleTestConfigurator.inc';
require_once 'FotowebBase.inc';
require_once 'FotowebRepresentationInterface.inc';
require_once 'FotowebCollection.inc';
require_once 'FotowebCollectionList.inc';
require_once 'FotowebArchives.inc';
require_once 'FotowebAssetList.inc';
require_once 'FotowebAsset.inc';

require_once 'FotowebArchiveAgentBase.inc';
require_once 'FotowebArchiveAgentOriginalImage.inc';

class FotowebBase {

  protected $server;
  protected $client;
  protected $entryPoint;
  protected $fullApiKey;

  public function __construct($server = NULL, $fullApiKey = NULL, FotowebGuzzleConfiguratorInterface $guzzleConfigurator = NULL) {
    if (empty($server)) {
      $this->setServer(variable_get('media_fotoweb_server'));
    }
    else {
      $this->setServer($server);
    }

    if (empty($fullApiKey)) {
      $this->setFullApiKey(variable_get('media_fotoweb_full_api_key'));
    }
    else {
      $this->setFullApiKey($fullApiKey);
    }

    if (empty($guzzleConfigurator)) {
      $guzzleConfigurator = new FotowebGuzzleConfigurator();
    }

    // For now we only support the private entry point.
    // @see: https://learn.fotoware.com/02_FotoWeb_8.0/Developing_with_the_FotoWeb_API/01_The_FotoWeb_RESTful_API/04_API_Entry_Points
    $this->entryPoint = FOTOWEB_PRIVATE_ENTRY_POINT;

    // Initialize Guzzle
    $guzzleConfiguration = $guzzleConfigurator::getConfiguration($this->getServer(), $this->getApiToken());
    $this->client = new GuzzleHttp\Client($guzzleConfiguration);
  }

  public function setServer($server) {
    $this->server = $server;
  }

  public function getServer() {
    return $this->server;
  }

  public function setFullApiKey($fullApiKey) {
    $this->fullApiKey = $fullApiKey;
  }

  public function getApiToken() {
    return $this->fullApiKey;
  }

  public function authenticate() {
    $response = $this->initiateAuthenticationRequest();
    if ($response->getStatusCode() !== 200) {
      throw new Exception('Authentication was not successful.', 403);
    }
    return $response;
  }

  public function initiateAuthenticationRequest() {
    $request = new GuzzleHttp\Psr7\Request('GET', '/fotoweb/me', [
      'Accept' => 'application/vnd.fotoware.full-api-descriptor+json',
      'FWAPITOKEN' => $this->getApiToken(),
    ]);

    return $this->initiateRequest($request);
  }

  public function initiateRequest($request) {
    // Create a fallback, when only the url as a string was provided.
    if (is_string($request)) {
      $request = new GuzzleHttp\Psr7\Request('GET', $request);
    }
    $response = $this->request($request);
    return $response;
  }

  protected function request($request) {
    $response = $this->client->send($request);
    return $response;
  }

}