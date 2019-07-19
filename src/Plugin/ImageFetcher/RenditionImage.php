<?php

namespace Drupal\media_fotoweb\Plugin\ImageFetcher;

use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\media_fotoweb\ImageFetcherBase;
use Drupal\media_fotoweb\FotowebClient;
use Drupal\media_fotoweb\RenditionNegotiator;

/**
 * Provides a Fotoweb Image Fetcher that loads a matching rendition image.
 *
 * @FotowebImageFetcher(
 *  id = "rendition_image",
 *  label = @Translation("Preview image"),
 *  weight = 5,
 * )
 */
class RenditionImage extends ImageFetcherBase {

  /**
   * The Fotoweb client.
   *
   * @var \Drupal\media_fotoweb\FotowebClient
   */
  protected $client;

  /**
   * The rendition negotiator.
   *
   * @var \Drupal\media_fotoweb\RenditionNegotiator
   */
  protected $renditionNegotiator;

  /**
   * RenditionImageFetcher constructor.
   *
   * @param array $configuration
   *   A configuration array containing information about the plugin instance.
   * @param string $plugin_id
   *   The plugin_id for the plugin instance.
   * @param string $plugin_definition
   *   The plugin implementation definition.
   * @param \Drupal\media_fotoweb\FotowebClient $client
   *   The Fotoweb client.
   * @param \Drupal\media_fotoweb\RenditionNegotiator $rendition_negotiator
   *   The rendition negotiator.
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, FotowebClient $client, RenditionNegotiator $rendition_negotiator
  ) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->client = $client;
    $this->renditionNegotiator = $rendition_negotiator;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('media_fotoweb.client'),
      $container->get('media_fotoweb.rendition_negotiator')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getImageByResourceUrl($resourceUrl) {
    try {
      $asset = $this->client->getAsset(['href' => $resourceUrl]);
      $bestFitImage = $this->renditionNegotiator->getBestFitImagePath($asset);
      $url = $bestFitImage['href'];
    }
    catch (\Exception $e) {
      return NULL;
    }
    $response = $this->client->client->getRendition($url);
    $body = (string) $response->getBody();
    return $body;
  }

}
