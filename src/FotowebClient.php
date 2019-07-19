<?php

namespace Drupal\media_fotoweb;

use Fotoweb\FotowebClient;
use Drupal\Core\Messenger\MessengerTrait;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Class FotowebClient.
 */
class FotowebClient {

  use MessengerTrait;
  use StringTranslationTrait;

  /**
   * The Guzzle Service Client.
   *
   * @var \GuzzleHttp\Command\ServiceClientInterface
   */
  public $client;

  protected $renditionService;

  /**
   * FotowebClient constructor.
   *
   * @param array $configuration
   *   The guzzle service configuration.
   */
  public function __construct(array $configuration) {
    $this->createClientFromConfiguration($configuration);
  }

  /**
   *
   */
  public function createClientFromConfiguration(array $configuration) {
    $this->client = new FotowebClient($configuration);
  }

  /**
   *
   */
  public function setRenditionService($rendition_service) {
    $this->renditionService = $rendition_service;
  }

  /**
   *
   */
  public function getRenditionService() {
    return $this->renditionService;
  }

  /**
   *
   */
  public function fetchApiDescriptor() {
    return $this->client->getApiDescriptor();
  }

  /**
   *
   */
  public function fetchRenditionService() {
    $renditionService = NULL;

    $apiDescriptor = $this->fetchApiDescriptor();
    if (!empty($apiDescriptor)) {
      $services = $apiDescriptor->offsetGet('services');

      if (!empty($services['rendition_request'])) {
        $renditionService = $services['rendition_request'];
      }
    }

    return $renditionService;
  }

  /**
   *
   */
  public function __call($name, $arguments) {
    return call_user_func_array([$this->client, $name], $arguments);
  }

}
