<?php

use GuzzleHttp\Client;
use GuzzleHttp\Psr7\Request;

define('FOTOWEB_AUTHENTICATION_QUERY_KEY', 'lt');

/**
 * FotowebArchiveAgentBase base for working with the Fotoweb ArchiveAgent.
 */
class FotowebArchiveAgentBase {

  protected $server;
  protected $client;
  protected $username;
  protected $encryptionSecret;

  /**
   * FotowebArchiveAgentBase constructor.
   *
   * @param string|null $server
   *   Fotoweb server url.
   * @param string|null $username
   *   Username used for authentication.
   * @param string|null $encryptionSecret
   *   Encryption secret for authentication.
   * @param FotowebGuzzleConfiguratorInterface|null $guzzleConfigurator
   *   Guzzle Configuration.
   */
  public function __construct($server = NULL, $username = NULL, $encryptionSecret = NULL, FotowebGuzzleConfiguratorInterface $guzzleConfigurator = NULL) {
    if (empty($server)) {
      $server = variable_get('media_fotoweb_server');
    }
    // Append ArchiveAgent paths.
    $server .= '/fotoweb/fwbin/fotoweb_isapi.dll/ArchiveAgent';
    $this->setServer($server);

    $this->setUsername($username);

    if (empty($encryptionSecret)) {
      $this->setEncryptionSecret(variable_get('media_fotoweb_encryption_secret'));
    }
    else {
      $this->setEncryptionSecret($encryptionSecret);
    }

    if (empty($guzzleConfigurator)) {
      $guzzleConfigurator = new FotowebGuzzleConfigurator();
    }

    // Initialize Guzzle.
    $guzzleConfiguration = $guzzleConfigurator::getConfiguration($this->getServer(), $this->getEncryptionSecret());
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
   * Sets the Fotoweb username.
   *
   * @param string $username
   *   Username used for authentication.
   */
  public function setUsername($username) {
    $this->username = $username;
  }

  /**
   * Returns the Fotoweb username.
   *
   * @return string
   *   Username used for authentication.
   */
  public function getUsername() {
    return $this->username;
  }

  /**
   * Sets the Fotoweb encryption secret.
   *
   * @param string $encryptionSecret
   *   Encryption secret for authentication.
   */
  public function setEncryptionSecret($encryptionSecret) {
    $this->encryptionSecret = $encryptionSecret;
  }

  /**
   * Returns the Fotoweb encryption secret.
   *
   * @return string
   *   Encryption secret for authentication.
   */
  public function getEncryptionSecret() {
    return $this->encryptionSecret;
  }

  /**
   * Initiates a HTTP request using Guzzle.
   *
   * @param string $url
   *   URL to HTTP request.
   *
   * @return mixed|\Psr\Http\Message\ResponseInterface
   *   HTTP response from the initiated request.
   */
  public function initiateRequest($url) {
    // Append authentication query parameter to request uri.
    // Only, when we have a proper fotoweb_username specified.
    if (!empty($this->getUsername())) {
      $url = $this->appendAuthenticationQuery($url);
    }
    $request = new Request('GET', $url);
    return $this->request($request);
  }

  /**
   * Actually send the prepared HTTP request.
   *
   * @param GuzzleHttp\Psr7\Request $request
   *   Guzzle Request object.
   *
   * @return mixed|\Psr\Http\Message\ResponseInterface
   *   HTTP response from the initiated request.
   */
  public function request(Request $request) {
    $response = $this->client->send($request);
    return $response;
  }

  /**
   * Appends the authentication query parameter to the current request.
   *
   * @param string $url
   *   Url to be requested.
   *
   * @return string
   *   Url with appended authentication string.
   */
  protected function appendAuthenticationQuery($url) {
    // Skip processing, if authentication query was already set.
    if (strpos($url, FOTOWEB_AUTHENTICATION_QUERY_KEY . '=') !== FALSE) {
      return $url;
    }
    if ($user_login_token = $this->getUserLoginToken()) {
      // Make a naive check, if query parameters were already set.
      if (strpos($url, '?') !== FALSE) {
        $url .= '&';
      }
      else {
        $url .= '?';
      }
      $url .= FOTOWEB_AUTHENTICATION_QUERY_KEY . '=' . $user_login_token;
    }
    return $url;
  }

  /**
   * Returns a login token for the current user for authenticating the API.
   *
   * @return string
   *   Login token.
   */
  protected function getUserLoginToken() {
    $tokenGenerator = new FotowebLoginTokenGenerator($this->getEncryptionSecret(), FALSE);
    return $tokenGenerator->CreateLoginToken($this->getUsername());
  }

}
