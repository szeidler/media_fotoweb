<?php

namespace Drupal\Tests\media_fotoweb\Unit;

use Drupal\Core\DependencyInjection\ContainerBuilder;
use Drupal\media_fotoweb\FotowebClient;
use Drupal\media_fotoweb\Plugin\ImageFetcher\RenditionImage;
use Drupal\media_fotoweb\RenditionNegotiator;
use Drupal\Tests\UnitTestCase;
use GuzzleHttp\Client;
use GuzzleHttp\Handler\MockHandler;
use GuzzleHttp\HandlerStack;
use GuzzleHttp\Middleware;
use GuzzleHttp\Psr7\Response;

/**
 * Tests the RenditionImage ImageFetcher
 *
 * @group media_fotoweb
 */
class RenditionImageFetcherTest extends UnitTestCase {

  /**
   * The rendition negotiator service.
   *
   * @var \Drupal\media_fotoweb\RenditionNegotiator
   */
  protected $renditionNegotiator;

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();

    $configFactory = $this
      ->getConfigFactoryStub([
        'media_fotoweb.settings' => [
          'local_file_size_threshold' => 1600,
        ],
      ]);
    $container = new ContainerBuilder();
    $container->set('config.factory', $configFactory);

    $this->renditionNegotiator = RenditionNegotiator::create($container);
  }

  /**
   * Builds a MockHandler for Fotoweb API communication.
   *
   * @param array $responses
   *   An array of mocked responses.
   * @param \GuzzleHttp\Handler\MockHandler|NULL $mockHandler
   *   The guzzle mock handler.
   *
   * @return \GuzzleHttp\Handler\MockHandler
   *   The modified guzzle mock handler.
   */
  protected function buildMockHandler(array $responses, MockHandler $mockHandler = NULL) {
    $mockHandler = $mockHandler ?: new MockHandler();
    foreach ($responses as $response) {
      $mockHandler->append($response);
    }

    return $mockHandler;
  }

  /**
   * Test getting a rendition image by Fotoweb resource URL.
   */
  public function testGetImageByResourceUrl() {
    $responses = [new Response(200, [], file_get_contents(__DIR__ . '/fixtures/asset.json')), new Response(200, [], 'myimage contents')];
    $mockHandler = $this->buildMockHandler($responses);

    // Use a history middleware to be able to verify which rendition image
    // was requested.
    $container = [];
    $history = Middleware::history($container);
    $stack = HandlerStack::create($mockHandler);
    $stack->push($history);

    $guzzle_client = new Client([
      'handler' => $stack,
    ]);

    $client = new FotowebClient([
      'client' => $guzzle_client,
      'baseUrl' => 'http://httpbin.org/',
    ]);

    // Get the rendition image from a resource URL.
    $renditionImageFetcher = new RenditionImage([], '', '', $client, $this->renditionNegotiator);
    $image = $renditionImageFetcher->getImageByResourceUrl('/fotoweb/archives/testarchive/myimage.jpg.info');
    $this->assertNotNull($image, 'Requestion a rendition image for an available asset should not return NULL.');

    // Verify that the expected preview image was requested.
    $transaction = array_pop($container);
    /** @var \GuzzleHttp\Psr7\Request $request */
    $request = $transaction['request'];
    $requestPath = $request->getUri()->getPath();
    $this->assertEquals('/fotoweb/cache/5000/myimage.t5653107a.m1600.xuETTbu2auFn8DaBE75CwjFgVNCFnggG4aSibC7uXR88.jpg', $requestPath, 'The request path matches the expected preview.');

    $image = $renditionImageFetcher->getImageByResourceUrl('/fotoweb/archives/testarchive/non-existing.jpg.info');
    $this->assertNull($image, 'Requesting a rendition image for a missing asset should return NULL.');
  }

}
