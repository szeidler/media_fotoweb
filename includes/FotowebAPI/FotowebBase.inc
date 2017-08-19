<?php

use GuzzleHttp\Client;
use GuzzleHttp\Psr7\Request;

define('FOTOWEB_PUBLIC_ENTRY_POINT', '/fotoweb');
define('FOTOWEB_PRIVATE_ENTRY_POINT', '/fotoweb/me/');

/**
 * FotowebBase is the base class for working with the Fotoweb RestAPI.
 */
class FotowebBase {

  protected $server;
  protected $client;
  protected $entryPoint;
  protected $fullApiKey;

  /**
   * FotowebBase constructor.
   *
   * @param string|null $server
   *   Fotoweb server url.
   * @param string|null $fullApiKey
   *   API key for authentication.
   * @param FotowebGuzzleConfiguratorInterface|null $guzzleConfigurator
   *   Guzzle Configuration.
   */
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

    // Initialize Guzzle.
    $guzzleConfiguration = $guzzleConfigurator::getConfiguration($this->getServer(), $this->getApiToken());
    $this->client = new Client($guzzleConfiguration);
  }

  /**
   * Sets the Fotoweb server.
   *
   * @param string $server
   *   Fotoweb server url.
   */
  public function setServer($server) {
    $this->server = $server;
  }

  /**
   * Returns the Fotoweb server.
   *
   * @return string
   *   Fotoweb server url.
   */
  public function getServer() {
    return $this->server;
  }

  /**
   * Sets the full API key.
   *
   * @param string $fullApiKey
   *   API key used for authentication.
   */
  public function setFullApiKey($fullApiKey) {
    $this->fullApiKey = $fullApiKey;
  }

  /**
   * Returns the API key.
   *
   * @return string
   *   API key for authentication.
   */
  public function getApiToken() {
    return $this->fullApiKey;
  }

  /**
   * Sends an authentication request to the API.
   *
   * @return mixed|\Psr\Http\Message\ResponseInterface
   *   HTTP response.
   *
   * @throws Exception
   */
  public function authenticate() {
    $response = $this->initiateAuthenticationRequest();
    if ($response->getStatusCode() !== 200) {
      throw new Exception('Authentication was not successful.', 403);
    }
    return $response;
  }

  /**
   * Initiate an authentication request to the API.
   *
   * @return mixed|\Psr\Http\Message\ResponseInterface
   *   HTTP response.
   */
  public function initiateAuthenticationRequest() {
    $request = new Request('GET', '/fotoweb/me', [
      'Accept' => 'application/vnd.fotoware.full-api-descriptor+json',
      'FWAPITOKEN' => $this->getApiToken(),
    ]);

    return $this->initiateRequest($request);
  }

  /**
   * Initiate an Fotoweb API request.
   *
   * @param string|GuzzleHttp\Psr7\Request $request
   *   URL string or Guzzle request object.
   *
   * @return mixed|\Psr\Http\Message\ResponseInterface
   *   HTTP response.
   */
  public function initiateRequest($request) {
    // Create a fallback, when only the url as a string was provided.
    if (is_string($request)) {
      $request = new Request('GET', $request);
    }
    $response = $this->request($request);
    return $response;
  }

  /**
   * Performs the actual HTTP request.
   *
   * @param GuzzleHttp\Psr7\Request $request
   *   Guzzle request object.
   *
   * @return mixed|\Psr\Http\Message\ResponseInterface
   *   HTTP response.
   */
  protected function request(Request $request) {
    $response = $this->client->send($request);
    return $response;
  }

}