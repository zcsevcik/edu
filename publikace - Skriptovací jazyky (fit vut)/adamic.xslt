<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="text" encoding="utf-8"/>

  <!-- get pubs -->
  <xsl:template match="/">
    <xsl:apply-templates select="//div[@class = 'item']/p"/>
  </xsl:template>

  <xsl:template match="p">
    <xsl:apply-templates/>
    <xsl:text>&#xa;&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="br"/>
  
  <xsl:template match="a[parent::p]">
    <!--<xsl:value-of select="text()"/>-->
    <xsl:apply-templates/>
    <xsl:text>.</xsl:text>
  </xsl:template>
  
  <!-- print content; output method=text removes tags -->
  <xsl:template match="@* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
