<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="text" encoding="utf-8"/>

  <!-- get pubs -->
  <xsl:template match="/">
    <xsl:apply-templates select="//table[@id='contenttable']//table//td[2]"/>
  </xsl:template>

  <!-- print url -->
  <xsl:template match="a[@href]" mode="url">
    <xsl:text>[url]</xsl:text>
    <xsl:value-of select="@href"/>
    <xsl:text>[/url]</xsl:text>
  </xsl:template>
  
  <!-- format pubs -->
  <xsl:template match="td">
    <xsl:apply-templates select="a[@href]" mode="url"/>
    <xsl:apply-templates select="node()"/>
    <xsl:text>&#xa;&#xa;</xsl:text>
  </xsl:template>
  
  <!-- print content; output method=text removes tags -->
  <xsl:template match="@* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
