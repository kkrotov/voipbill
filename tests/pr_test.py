#!/bin/python
# -*- coding: utf-8 -*-

import xmlrunner
import unittest2
import psycopg2

class TestPullRequest(unittest2.TestCase):

  def test_pullRequest(self):

    self.assertTrue(True)

# Результаты теста пишем в Jenkins, чтобы было видно в отчёте

if __name__ == '__main__':
  unittest2.main(testRunner=xmlrunner.XMLTestRunner(output='.'))

