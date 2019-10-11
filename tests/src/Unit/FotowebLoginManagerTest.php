<?php

namespace Drupal\Tests\media_fotoweb\Unit;

use Drupal\Core\DependencyInjection\ContainerBuilder;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Session\AccountProxyInterface;
use Drupal\media_fotoweb\FotowebLoginManager;
use Drupal\media_fotoweb\RenditionNegotiator;
use Drupal\Tests\UnitTestCase;
use Fotoweb\Representation\Asset;

/**
 * Tests the FotowebLoginManager.
 *
 * @group media_fotoweb
 */
class FotowebLoginManagerTest extends UnitTestCase {

  /**
   * The Fotoweb Login Manager service.
   *
   * @var \Drupal\media_fotoweb\FotowebLoginManager
   */
  protected $fotowebLoginManager;

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();

    $config_factory = $this
      ->getConfigFactoryStub([
        'media_fotoweb.settings' => [
          'sso_user_field' => 'name',
          'encryption_secret' => '4ff792cd7f1132cdce40f2da0c437ee4',
        ],
      ]);
    $container = new ContainerBuilder();
    $container->set('config.factory', $config_factory);

    // Create mocks.
    $fieldItem = $this->getMockBuilder(FieldItemListInterface::class)
      ->disableOriginalConstructor()
      ->getMock();
    $fieldItem->expects($this->any())
      ->method('__get')
      ->with('value')
      ->willReturn('fotoweb_testuser');

    $testUser = $this->getMockBuilder('Drupal\user\Entity\User')
      ->disableOriginalConstructor()
      ->getMock();
    $testUser->expects($this->any())
      ->method('hasField')
      ->with('name')
      ->will($this->returnValue(TRUE));
    $testUser->expects($this->any())
      ->method('get')
      ->with('name')
      ->will($this->returnValue($fieldItem));

    $userStorage = $this->createMock('Drupal\Core\Entity\EntityStorageInterface');
    $userStorage->expects($this->any())
      ->method('load')
      ->will($this->returnValue($testUser));

    $entity_type_manager = $this->createMock(EntityTypeManagerInterface::class);
    $entity_type_manager->expects($this->any())
      ->method('getStorage')
      ->with('user')
      ->will($this->returnValue($userStorage));

    $this->fotowebLoginManager = new FotowebLoginManager($config_factory, $entity_type_manager);
  }

  /**
   * Tests to get the user login token by username.
   */
  public function testGetUserLoginTokenFromUsername() {
    $username = 'fotoweb_testuser';
    // The login token uses time based components, so we are only checking
    // for the expected format.
    $this->assertRegExp('/^[A-Za-z0-9]{0,130}==$/', $this->fotowebLoginManager->getUserLoginTokenFromUsername($username));
  }

  /**
   * Tests to get the token from an account.
   */
  public function testGetLoginTokenFromAccount() {
    $account = $this->createMock(AccountProxyInterface::class);
    $account->expects($this->any())
      ->method('id')
      ->will($this->returnValue(1));

    $this->assertRegExp('/^[A-Za-z0-9]{0,130}==$/', $this->fotowebLoginManager->getLoginTokenFromAccount($account));
  }

}
