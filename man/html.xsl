<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    version="1.0">

  <xsl:import href="http://docbook.sourceforge.net/release/xsl/current/html/docbook.xsl"/>
  <xsl:include href="common.xsl"/>

  <xsl:output
      method="html"
      encoding="UTF-8"
      indent="yes"/>

  <xsl:param name="citerefentry.link">1</xsl:param>
  <xsl:param name="funcsynopsis.style">ansi</xsl:param>
  <xsl:param name="html.stylesheet">manpage.css</xsl:param>

  <!--
   ! Generate urls for citations.
   !-->
  <xsl:template name="generate.citerefentry.link">
    <xsl:choose>

      <!--
       ! Link EGL citations to
       !   href="http://www.khronos.org/registry/egl/sdk/docs/man/xhtml/$refentrytitle.html".
       !-->
      <xsl:when test="starts-with(refentrytitle, 'egl')">
        <xsl:text>http://www.khronos.org/registry/egl/sdk/docs/man/xhtml/</xsl:text>
        <xsl:value-of select="refentrytitle"/>
        <xsl:text>.html</xsl:text>
      </xsl:when>

      <!--
       ! Link GLX citations to
       ! href="http://www.opengl.org/sdk/docs/man2/xhtml/$refentrytitle.xml".
       !-->
      <xsl:when test="starts-with(refentrytitle, 'glX')">
        <xsl:text>http://www.opengl.org/sdk/docs/man2/xhtml/</xsl:text>
        <xsl:value-of select="refentrytitle"/>
        <xsl:text>.xml</xsl:text>
      </xsl:when>

      <!--
       ! If a citation's title begins with "waffle_$x", then link it with
       ! href="waffle_$x.$manvolnum.html". For example, link a citation
       ! to waffle_context_create(3) with href="waffle_context.3.html".
       !-->

      <!-- sorted alphabetically -->
      <xsl:when test="starts-with(refentrytitle, 'waffle_attrib_list')">
        <xsl:text>waffle_attrib_list.3.html</xsl:text>
      </xsl:when>
      <xsl:when test="starts-with(refentrytitle, 'waffle_config')">
        <xsl:text>waffle_config.3.html</xsl:text>
      </xsl:when>
      <xsl:when test="starts-with(refentrytitle, 'waffle_context')">
        <xsl:text>waffle_context.3.html</xsl:text>
      </xsl:when>
      <xsl:when test="starts-with(refentrytitle, 'waffle_display')">
        <xsl:text>waffle_display.3.html</xsl:text>
      </xsl:when>
      <xsl:when test="starts-with(refentrytitle, 'waffle_dl')">
        <xsl:text>waffle_dl.3.html</xsl:text>
      </xsl:when>
      <xsl:when test="starts-with(refentrytitle, 'waffle_enum')">
        <xsl:text>waffle_enum.3.html</xsl:text>
      </xsl:when>
      <xsl:when test="starts-with(refentrytitle, 'waffle_error')">
        <xsl:text>waffle_error.3.html</xsl:text>
      </xsl:when>
      <xsl:when test="starts-with(refentrytitle, 'waffle_gbm')">
        <xsl:text>waffle_gbm.3.html</xsl:text>
      </xsl:when>
      <xsl:when test="starts-with(refentrytitle, 'waffle_glx')">
        <xsl:text>waffle_glx.3.html</xsl:text>
      </xsl:when>
      <xsl:when test="starts-with(refentrytitle, 'waffle_native')">
        <xsl:text>waffle_native.3.html</xsl:text>
      </xsl:when>
      <xsl:when test="starts-with(refentrytitle, 'waffle_wayland')">
        <xsl:text>waffle_wayland.3.html</xsl:text>
      </xsl:when>
      <xsl:when test="starts-with(refentrytitle, 'waffle_window')">
        <xsl:text>waffle_window.3.html</xsl:text>
      </xsl:when>
      <xsl:when test="starts-with(refentrytitle, 'waffle_xegl')">
        <xsl:text>waffle_xegl.3.html</xsl:text>
      </xsl:when>

      <!--
       ! If a citation's title begins with "wflinfo", then link it with
       ! href="wflinfo.$manvolnum.html".
       !-->
      <xsl:when test="starts-with(refentrytitle, 'wflinfo')">
        <xsl:text>wflinfo.1.html</xsl:text>
      </xsl:when>

      <!--
       ! For citations that begin with "waffle" but were not matched above,
       ! we assume that the citation title is the refentrytitle of a
       ! manpage. Link with href="$refentrytitle.$manvolnum.html".
       !-->
      <xsl:when test="starts-with(refentrytitle, 'waffle')">
        <xsl:value-of select="refentrytitle"/>
        <xsl:text>.</xsl:text>
        <xsl:value-of select="manvolnum"/>
        <xsl:text>.html</xsl:text>
      </xsl:when>

      <!--
       ! Link all unmatched citations with
       ! href="http://linuxmanpages.net/manpages/fedora17/man/$manvolnum/$refentrytitle.html".
       !-->
      <xsl:otherwise>
        <xsl:text>http://linuxmanpages.net/manpages/fedora17/man</xsl:text>
        <xsl:value-of select="manvolnum"/>
        <xsl:text>/</xsl:text>
        <xsl:value-of select="refentrytitle"/>
        <xsl:text>.</xsl:text>
        <xsl:value-of select="manvolnum"/>
        <xsl:text>.html</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>

<!--
  vim:sw=2 ts=2 et:
-->
