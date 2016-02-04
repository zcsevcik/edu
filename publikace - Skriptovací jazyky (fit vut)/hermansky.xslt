<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="text" encoding="utf-8"/>

  <!-- get pubs -->
  <xsl:template match="/">
    <xsl:apply-templates select="//div[@id = 'publications-container']/div"/>
  </xsl:template>

  <xsl:template name="bib">
    <xsl:param name="text"/>

    <xsl:text>[bib]</xsl:text>
    <xsl:value-of select="$text"/>
    <xsl:text>[/bib]</xsl:text>
  </xsl:template>

  <xsl:template match="br">
    <xsl:text>. </xsl:text>
  </xsl:template>

  <xsl:template match="em">
    <xsl:value-of select="translate(., '–', ',')"/>
  </xsl:template>
  
  <xsl:template match="p">
    <xsl:apply-templates select="a[@class = 'title-link']" mode="url"/>
    <xsl:apply-templates select="node()"/>
  </xsl:template>

  <xsl:template match="a[@class = 'title-link']" mode="url">
    <xsl:text>[url]</xsl:text>
    <xsl:value-of select="@href"/>
    <xsl:text>[/url]</xsl:text>
  </xsl:template>
    
  <xsl:template match="div">
    <xsl:call-template name="bib">
      <xsl:with-param name="text" select="div[contains(@class, 'bib')]"/>
    </xsl:call-template>

    <xsl:apply-templates select="p[1]"/>

    <xsl:text>&#xa;&#xa;</xsl:text>
  </xsl:template>

  <!-- print content; output method=text removes tags -->
  <xsl:template match="@* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
