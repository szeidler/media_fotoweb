<?php

namespace Drupal\Tests\media_fotoweb\Unit;

use Drupal\Core\DependencyInjection\ContainerBuilder;
use Drupal\media_fotoweb\RenditionNegotiator;
use Drupal\Tests\UnitTestCase;
use Fotoweb\Representation\Asset;

/**
 * Tests the RenditionNegotiator.
 *
 * @group media_fotoweb
 */
class RenditionNegotiatorTest extends UnitTestCase {

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
   * Tests the image is a better fit check.
   */
  public function testIsImageABetterFit() {
    $originalImageWidth = 4000;

    $currentImage = ['width' => 1600, 'square' => FALSE];
    $bestFitImage = ['width' => 1400, 'square' => FALSE];
    $this->assertTrue($this->renditionNegotiator->isImageABetterFit($currentImage, $bestFitImage, $originalImageWidth));

    $currentImage = ['width' => 1601, 'square' => FALSE];
    $bestFitImage = ['width' => 1600, 'square' => FALSE];
    $this->assertFalse($this->renditionNegotiator->isImageABetterFit($currentImage, $bestFitImage, $originalImageWidth));

    $currentImage = ['width' => 1700, 'square' => FALSE];
    $bestFitImage = ['width' => 1500, 'square' => FALSE];
    $this->assertTrue($this->renditionNegotiator->isImageABetterFit($currentImage, $bestFitImage, $originalImageWidth));

    $currentImage = ['width' => 1700, 'square' => FALSE];
    $bestFitImage = ['width' => 1650, 'square' => FALSE];
    $this->assertFalse($this->renditionNegotiator->isImageABetterFit($currentImage, $bestFitImage, $originalImageWidth));
  }

  /**
   * Test ordering previews ascendant.
   */
  public function testOrderPreviewsAscendant() {
    $previews = [
      ['width' => 500],
      ['width' => 1600],
      ['width' => -1400],
      ['width' => 1450],
      ['width' => 1800],
      ['width' => 5000],
      ['width' => 1720],
    ];

    $expected = [
      ['width' => -1400],
      ['width' => 500],
      ['width' => 1450],
      ['width' => 1600],
      ['width' => 1720],
      ['width' => 1800],
      ['width' => 5000],
    ];

    $this->assertEquals($expected, $this->renditionNegotiator->orderPreviewsAscendant($previews));
  }

  /**
   * Test ordering previews descendant.
   */
  public function testOrderPreviewsDescendant() {
    $previews = [
      ['width' => 500],
      ['width' => 1600],
      ['width' => -1400],
      ['width' => 1450],
      ['width' => 1800],
      ['width' => 5000],
      ['width' => 1720],
    ];

    $expected = [
      ['width' => 5000],
      ['width' => 1800],
      ['width' => 1720],
      ['width' => 1600],
      ['width' => 1450],
      ['width' => 500],
      ['width' => -1400],
    ];

    $this->assertEquals($expected, $this->renditionNegotiator->orderPreviewsDescendant($previews));
  }

  /**
   * Test the full get bet fit image procedure.
   */
  public function testGetBestFitImagePath() {
    // Test an asset without previews.
    $data = ['href' => 'identifier'];
    $asset = new Asset($data);
    $this->assertNull($this->renditionNegotiator->getBestFitImagePath($asset), 'An asset without preview properties should return NULL');

    // Test that an asset with correct formatted previews returns the best fit.
    $data = [
      'attributes' => [
        'imageattributes' => [
          'pixelwidth' => 3000,
        ],
      ],
      'previews' => [
        ['width' => 500, 'square' => FALSE],
        ['width' => 1600, 'square' => TRUE],
        ['width' => 1400, 'square' => TRUE],
        ['width' => 1450, 'square' => FALSE],
        ['width' => 1800, 'square' => FALSE],
        ['width' => 5000, 'square' => FALSE],
        ['width' => 1720, 'square' => FALSE],
      ],
    ];
    $asset = new Asset($data);
    $bestFitImage = ['width' => 1720, 'square' => FALSE];
    $this->assertEquals($bestFitImage, $this->renditionNegotiator->getBestFitImagePath($asset));
  }

  /**
   * Test without a threshold the biggest preview should be returned.
   */
  public function testEmptyTresholdReturnsBiggestPreview() {
    $configFactory = $this
      ->getConfigFactoryStub([
        'media_fotoweb.settings' => [
          'local_file_size_threshold' => 1600,
        ],
      ]);
    $container = new ContainerBuilder();
    $container->set('config.factory', $configFactory);

    $this->renditionNegotiator = RenditionNegotiator::create($container);

    // Test that an asset with correct formatted previews returns the best fit.
    $data = [
      'attributes' => [
        'imageattributes' => [
          'pixelwidth' => 3000,
        ],
      ],
      'previews' => [
        ['width' => 500],
        ['width' => 1700],
        ['width' => 1400],
        ['width' => 1450],
        ['width' => 1800],
        ['width' => 5000],
        ['width' => 1720],
      ],
    ];
    $asset = new Asset($data);
    $bestFitImage = ['width' => 1700];
    $this->assertEquals($bestFitImage, $this->renditionNegotiator->getBestFitImagePath($asset));
  }

}
