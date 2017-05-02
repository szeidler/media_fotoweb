<?php

DEFINE('FOTOWEB_AUTHENTICATION_QUERY_KEY', 'lt');

require_once 'FotowebBase.inc';

class FotowebArchiveAgentBase {

  protected $server;
  protected $client;
  protected $username;
  protected $encryptionSecret;

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

    // Initialize Guzzle
    $guzzleConfiguration = $guzzleConfigurator::getConfiguration($this->getServer(), $this->getEncryptionSecret());
    $this->client = new GuzzleHttp\Client($guzzleConfiguration);
  }

  public function setServer($server) {
    $this->server = $server;
  }

  public function getServer() {
    return $this->server;
  }

  public function setUsername($username) {
    $this->username = $username;
  }

  public function getUsername() {
    return $this->username;
  }

  public function setEncryptionSecret($encryptionSecret) {
    $this->encryptionSecret = $encryptionSecret;
  }

  public function getEncryptionSecret() {
    return $this->encryptionSecret;
  }

  public function initiateRequest($url) {
    // Append authentication query parameter to request uri.
    // Only, when we have a proper fotoweb_username specified.
    if (!empty($this->getUsername())) {
      $url = $this->appendAuthenticationQuery($url);
    }
    $request = new GuzzleHttp\Psr7\Request('GET', $url);
    return $this->request($request);
  }

  public function request($request) {
    $response = $this->client->send($request);
    return $response;
  }

  protected function appendAuthenticationQuery($url) {
    // Skip processing, if authentication query was already set.
    if (strpos($url, FOTOWEB_AUTHENTICATION_QUERY_KEY . '=') !== FALSE) return $url;
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
   **
   * @return string
   */
  protected function getUserLoginToken() {
    $tokenGenerator = new FotowebLoginTokenGenerator($this->getEncryptionSecret(), FALSE);
    return $tokenGenerator->CreateLoginToken($this->getUsername());
  }

}