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

  /**
   * The rendition service URL.
   *
   * @var string
   */
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
   * Creates a FotowebClient from configuration.
   *
   * @param array $configuration
   *   A Fotoweb client configuration array.
   */
  public function createClientFromConfiguration(array $configuration) {
    $this->client = new FotowebClient($configuration);
  }

  /**
   * Sets the rendition service.
   *
   * @param string $rendition_service
   *   The rendition service URL.
   */
  public function setRenditionService($rendition_service) {
    $this->renditionService = $rendition_service;
  }

  /**
   * Returns the rendition service.
   *
   * @return string
   *   The rendition service URL.
   */
  public function getRenditionService() {
    return $this->renditionService;
  }

  /**
   * Fetches the API Descriptor.
   *
   * @return mixed
   *   The API descriptor.
   */
  public function fetchApiDescriptor() {
    return $this->client->getApiDescriptor();
  }

  /**
   * Fetches the renditions ervice.
   *
   * @return mixed|null
   *   The rendition service or NULL on failur.
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
   * Forward methods to the Fotoweb Client.
   *
   * @param string $name
   *   Name of the method.
   * @param array $arguments
   *   Method arguments.
   *
   * @return mixed
   *   The method return value.
   */
  public function __call($name, array $arguments) {
    return call_user_func_array([$this->client, $name], $arguments);
  }

}
