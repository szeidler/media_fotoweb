<?php

namespace Drupal\media_fotoweb\Plugin\ImageFetcher;

use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\media_fotoweb\ImageFetcherBase;
use Drupal\media_fotoweb\FotowebClient;

/**
 * @FotowebImageFetcher(
 *  id = "original_image",
 *  label = @Translation("Original image"),
 *   weight = 0,
 * )
 */
class OriginalImage extends ImageFetcherBase {

  /**
   * The Fotoweb client.
   *
   * @var \Drupal\media_fotoweb\FotowebClient
   */
  protected $client;

  /**
   * OriginalImageFetcher constructor.
   *
   * @param array $configuration
   *   A configuration array containing information about the plugin instance.
   * @param string $plugin_id
   *   The plugin_id for the plugin instance.
   * @param string $plugin_definition
   *   The plugin implementation definition.
   * @param \Drupal\media_fotoweb\FotowebClient $client
   *   The Fotoweb client.
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, FotowebClient $client) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->client = $client;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('media_fotoweb.client')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getImageByResourceUrl($resourceUrl) {
    try {
      $asset = $this->client->getAsset(['href' => $resourceUrl]);
      $rendition_resource = $asset->offsetGet('renditions')[0]['href'];
      $rendition_request = $this->client->client->getRenditionRequest([
        'rendition_service' => $this->client->getRenditionService(),
        'href' => $rendition_resource,
      ]);
      $url = $rendition_request->getHref();
    }
    catch (\Exception $e) {
      return NULL;
    }
    $response = $this->client->client->getRendition($url);
    $body = (string) $response->getBody();
    return $body;
  }

}
