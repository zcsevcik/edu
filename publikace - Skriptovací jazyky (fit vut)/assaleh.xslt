<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="text" encoding="utf-8"/>

  <!-- get pubs -->
  <xsl:template match="/">
    <xsl:apply-templates select="//div[@id = 'primary']//div[@class = 'entry-content']//ol/li"/>
  </xsl:template>

  <xsl:template match="li">
    <xsl:apply-templates select="div[@class = 'bibtex']"/>
    <xsl:apply-templates select="div[not(@class)]"/>
    <xsl:text>&#xa;&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="div[@class = 'bibtex']">
    <xsl:text>[bib]</xsl:text>
    <!-- 0xC2A0 -> 0x20 -->
    <xsl:value-of select="translate(code, ' ', ' ')"/>
    <xsl:text>[/bib]</xsl:text>
  </xsl:template>

  <xsl:template match="code">
    <!--<xsl:value-of select="translate(text(), ' ', ' ')"/>-->
  </xsl:template>

  <xsl:template match="div">
    <xsl:apply-templates select="node()[following-sibling::br]"/>
  </xsl:template>

  <!-- print content; output method=text removes tags -->
  <xsl:template match="@* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
