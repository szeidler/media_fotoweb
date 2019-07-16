<?php

namespace Drupal\media_fotoweb;

use Drupal\Core\Session\AccountInterface;

/**
 * Interface FotowebLoginManagerInterface.
 */
interface FotowebLoginManagerInterface {

  /**
   * Returns the Fotoweb Login Token for a Drupal user.
   *
   * @param \Drupal\Core\Session\AccountInterface $user
   *   The user object.
   *
   * @return string
   *   The Fotoweb Login Token.
   */
  public function getLoginTokenFromUser(AccountInterface $user);

}
