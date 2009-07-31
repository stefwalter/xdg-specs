<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
        xmlns:tp="http://telepathy.freedesktop.org/wiki/DbusSpec#extensions-v0"
        exclude-result-prefixes="tp">

<!--
    Telepathy D-Bus Introspection to EggDBus Introspection format translator.

    Copyright 2009  Michael Leupold <lemma@confuego.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
-->

<!--
    TODO:
     - Enable conversion of dictionary element types (eg. "{ss}") and
       struct types (eg. "(sayay)")
     - unhandled: tp:simple-type, tp:enum, tp:flags, tp:external-type
     - tp:docstring may contain XHTML which this template doesn't handle
-->

    <!-- main template -->
    <xsl:template match="tp:spec">
        <node>
            <xsl:apply-templates select="tp:errors"/>
            <xsl:apply-templates select="tp:struct"/>
            <!-- TODO: <xsl:apply-templates select="tp:mapping"/> -->
            <xsl:apply-templates select="node/interface"/>
        </node>
    </xsl:template>

    <!-- Resolve the type a node has. This will first look at tp:type and
         - if not found - use the type attribute -->
    <xsl:template name="ResolveType">
        <xsl:param name="node"/>
        <xsl:choose>
            <xsl:when test="$node//@tp:type">
                <xsl:call-template name="TpType">
                    <xsl:with-param name="type" select="$node//@tp:type"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:when test="$node//@type">
                <xsl:call-template name="DBusType">
                    <xsl:with-param name="type" select="$node//@type"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:message terminate="yes">
                    Node doesn't contain a type.
                </xsl:message>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- Map a D-Bus type to its EggDBus counterpart -->
    <xsl:template name="DBusType">
        <xsl:param name="type"/>
        <xsl:choose>
            <xsl:when test="$type='o'">ObjectPath</xsl:when>
            <xsl:when test="$type='s'">String</xsl:when>
            <xsl:when test="$type='y'">Byte</xsl:when>
            <xsl:when test="$type='b'">Boolean</xsl:when>
            <xsl:when test="$type='n'">Int16</xsl:when>
            <xsl:when test="$type='q'">UInt16</xsl:when>
            <xsl:when test="$type='i'">Int32</xsl:when>
            <xsl:when test="$type='u'">UInt32</xsl:when>
            <xsl:when test="$type='x'">Int64</xsl:when>
            <xsl:when test="$type='t'">UInt64</xsl:when>
            <xsl:when test="$type='d'">Double</xsl:when>
            <xsl:when test="$type='g'">Signature</xsl:when>
            <xsl:when test="starts-with($type, 'a')">
                Array&lt;
                <xsl:call-template name="DBusType">
                    <xsl:with-param name="type" select="substring($type, 2)"/>
                </xsl:call-template>
                &gt;
            </xsl:when>
            <!-- TODO: doesn't implement dict-entries and structs -->
            <xsl:otherwise>
                <xsl:message terminate="yes">
                    Unknown DBus Type <xsl:value-of select="$type"/>
                </xsl:message>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- Resolve tp:type attributes by searching for matching tp:struct
         and tp:mapping elements -->
    <xsl:template name="TpType">
        <xsl:param name="type"/>
        <xsl:choose>
            <xsl:when test="/tp:spec/tp:struct[@name=$type]">
                <xsl:value-of select="$type"/>
            </xsl:when>
            <xsl:when test="/tp:spec/tp:mapping[@name=$type]">
                Dict&lt;
                <xsl:call-template name="ResolveType">
                    <xsl:with-param name="node" select="/tp:spec/tp:mapping[@name=$type]/tp:member[@name='Key']"/>
                </xsl:call-template>,
                <xsl:call-template name="ResolveType">
                    <xsl:with-param name="node" select="/tp:spec/tp:mapping[@name=$type]/tp:member[@name='Value']"/>
                </xsl:call-template>
                &gt;
            </xsl:when>
            <xsl:otherwise>
                <xsl:message terminate="yes">
                    Unspecified type <xsl:value-of select="$type"/>.
                </xsl:message>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- handle most of the D-Bus introspection elements -->
    <xsl:template match="interface|annotation|method|signal">
        <xsl:copy>
            <xsl:for-each select="@*">
                <xsl:if test="not(starts-with(name(), 'tp:'))">
                    <xsl:copy/>
                </xsl:if>
            </xsl:for-each>
            <xsl:apply-templates/>
        </xsl:copy>
    </xsl:template>

    <!-- handle the arg and property D-Bus introspection elements.
         They get special handling because they may contain a tp:type
         attribute -->
    <xsl:template match="arg|property">
        <xsl:copy>
            <xsl:for-each select="@*">
                <xsl:choose>
                    <xsl:when test="not(starts-with(name(), 'tp:'))">
                        <xsl:copy/>
                    </xsl:when>
                    <xsl:when test="name() = 'tp:type'">
                        <xsl:variable name="type">
                            <xsl:call-template name="TpType">
                                <xsl:with-param name="type" select="."/>
                            </xsl:call-template>
                        </xsl:variable>
                        <annotation name="org.gtk.EggDBus.Type">
                            <xsl:attribute name="value">
                                <xsl:value-of select="normalize-space($type)"/>
                            </xsl:attribute>
                        </annotation>
                    </xsl:when>
                    <xsl:otherwise/>
                </xsl:choose>
            </xsl:for-each>
            <xsl:apply-templates/>
        </xsl:copy>
    </xsl:template>

    <!-- tp:docstring to org.gtk.EggDBus.DocString -->
    <xsl:template match="tp:docstring">
        <annotation name="org.gtk.EggDBus.DocString">
            <xsl:attribute name="value">
                <xsl:value-of select="normalize-space(text())"/>
            </xsl:attribute>
        </annotation>
    </xsl:template>

    <!-- tp:errors to org.gtk.EggDBus.DeclareErrorDomain -->
    <xsl:template match="tp:errors">
        <annotation value="Error" name="org.gtk.EggDBus.DeclareErrorDomain">
            <xsl:apply-templates select="tp:docstring"/>
            <xsl:apply-templates select="tp:error"/>
        </annotation>
    </xsl:template>

    <!-- tp:error to org.gtk.EggDBus.ErrorDomain.Member -->
    <xsl:template match="tp:error">
        <annotation name="org.gtk.EggDBus.ErrorDomain.Member">
            <xsl:attribute name="value">
                <xsl:value-of select="concat(../@namespace, '.', @name)"/>
            </xsl:attribute>
            <xsl:apply-templates select="tp:docstring"/>
        </annotation>
    </xsl:template>

    <!-- tp:struct to org.gtk.EggDBus.DeclareStruct -->
    <xsl:template match="tp:struct">
        <annotation name="org.gtk.EggDBus.DeclareStruct">
            <xsl:attribute name="value">
                <xsl:value-of select="@name"/>
            </xsl:attribute>
            <xsl:apply-templates select="tp:docstring"/>
            <xsl:apply-templates select="tp:member"/>
        </annotation>
    </xsl:template>

    <!-- tp:member to org.gtk.EggDBus.Struct.Member -->
    <xsl:template match="tp:member">
        <xsl:variable name="type">
            <xsl:call-template name="ResolveType">
                <xsl:with-param name="node" select="."/>
            </xsl:call-template>
        </xsl:variable>
        <annotation name="org.gtk.EggDBus.Struct.Member">
            <xsl:attribute name="value">
                <xsl:value-of select="concat(normalize-space($type), ':', @name)"/>
            </xsl:attribute>
            <xsl:apply-templates select="tp:docstring"/>
        </annotation>
    </xsl:template>

    <xsl:template match="text()"/>

    <xsl:output method="xml" indent="yes" encoding="UTF-8"
                omit-xml-declaration="no"
                doctype-system="http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd"
                doctype-public="-//freedesktop//DTD D-BUS Object Introspection 1.0//EN"/>

</xsl:stylesheet>
