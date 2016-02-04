<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="text" encoding="utf-8"/>

  <!-- get pubs -->
  <xsl:template match="/">
    <xsl:apply-templates select="//pre"/>
  </xsl:template>

  <xsl:template match="pre">
    <xsl:choose>
      <xsl:when test="not(br)">
        <xsl:value-of select="normalize-space(text())"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="normalize-space(text()[preceding-sibling::br])"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  
  <!-- print content; output method=text removes tags -->
  <xsl:template match="@* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
