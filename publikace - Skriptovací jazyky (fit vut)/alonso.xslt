<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="text" encoding="utf-8"/>

  <!-- get pubs -->
  <xsl:template match="/">
    <xsl:apply-templates select="//ul[position() > 2]/li"/>
  </xsl:template>

  <xsl:template match="li">
    <xsl:apply-templates select="a[preceding-sibling::br]" mode="reference"/>
    <xsl:apply-templates select="node()[following-sibling::br]"/>
    <xsl:text>&#xa;&#xa;</xsl:text>
  </xsl:template>
  
  <xsl:template match="b">
    <xsl:text>"</xsl:text>
    <xsl:value-of select="."/>
    <xsl:text>"</xsl:text>
  </xsl:template>
  
  <xsl:template match="a" mode="reference">
    <xsl:choose>
      <xsl:when test="substring(@href, string-length(@href) - 3) = '.pdf'">
        <xsl:text>[url]</xsl:text>
        <xsl:value-of select="@href"/>
        <xsl:text>[/url]</xsl:text>
      </xsl:when>
      <xsl:when test="substring(@href, string-length(@href) - 3) = '.bib'">
        <xsl:text>[bib]</xsl:text>
        <xsl:value-of select="@href"/>
        <xsl:text>[/bib]</xsl:text>
      </xsl:when>
    </xsl:choose>
    
  </xsl:template>

  <!-- print content; output method=text removes tags -->
  <xsl:template match="@* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
