<?php
/**
 * @file
 * This file contains all theme and preprocess functions for
 * islandora_pbcore_display.
 */

/**
 * Implements hook_preprocess().
 */
function islandora_pbcore_display_preprocess_islandora_pbcore_display_display(array &$variables) {
  $object = $variables['islandora_object'];

  if ($object['PBCORE']) {
    try {
      $pbcore_o = $object['PBCORE'];
      if ($pbcore_o) {
        $pbcore = $pbcore_o->content;
      }
    } catch (Exception $e) {
      drupal_set_message(t('Error retrieving object %s %t', array(
        '%s' => $object->id,
        '%t' => $e->getMessage(),
      )), 'error', FALSE);
      $variables['metadata'] = '';
    }
  }

  if (! isset($pbcore)) {
    drupal_set_message(t('PBCORE description not found for %s', array(
      '%s' => $object->id,
    )), 'error', FALSE);
    $variables['metadata'] = '';
  }
  else {
    $pbcore_dom = new DOMDocument();
    $did_load_ok = $pbcore_dom->loadXML($pbcore);
    if ($did_load_ok) {
      $display = array();
      $ini_file = drupal_get_path('module', 'islandora_pbcore_display') . '/pbcore_display.ini';
      $ini_array = parse_ini_file($ini_file, TRUE);
      if (!$ini_array) {
        drupal_set_message(t('Error retrieving islandora_pbcore_display config file'), 'error', FALSE);
      }
      else {
        $pbcore_xpath = new DOMXpath($pbcore_dom);
        foreach ($ini_array as $section_title => $section_array) {
          $display[] = "<h2>$section_title</h2>";
          $display[] = '<dl class="islandora-inline-metadata islandora-metadata-fields">';
          foreach ($section_array as $key => $xpath) {
            $elements = $pbcore_xpath->query($xpath);
            if ($elements === FALSE) {
              drupal_set_message(t('Error in xpath for %s:%k', array(
                '%s' => $section_title,
                '%k' => $key,
              )), 'error', FALSE);
            }
            elseif ($elements->item(0) && $elements->item(0)->nodeValue) {
              $display[] = "<dt>$key</dt>";
              $display[] = '<dd>';
              foreach ($elements as $value) {
                $display[] = "$value->nodeValue<br>";
              }
              $display[] = '</dd>';
            }
          }
          $display[] = '</dl>';
        }
      }
      $variables['metadata'] = join("\n", $display);
    }
  }
}

/**
 * Implements hook_preprocess().
 */
function islandora_pbcore_display_preprocess_islandora_pbcore_display_description(array &$variables) {
  $object = $variables['islandora_object'];
  // print_r($object);
  $variables['description'] = t('@object', array(
    '@object' => $object->label,
  ));
}
