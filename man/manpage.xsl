<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    version="1.0">

  <xsl:import href="http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl"/>
  <xsl:include href="common.xsl"/>

  <!-- Don't print name of each manpage generated. -->
  <xsl:param name="man.output.quietly">1</xsl:param>

</xsl:stylesheet>

<!--
  vim:sw=2 ts=2 et:
-->
