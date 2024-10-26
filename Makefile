include $(TOPDIR)/rules.mk

PKG_NAME:=esp
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/esp
	DEPENDS:=+libjson-c +libtuya +libubus +libubox +libblobmsg-json +libserialport
	CATEGORY:=Base system
	TITLE:=UBUS module for controlling ESP micro controller
endef

define Package/esp/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/esp $(1)/usr/bin/esp
	$(INSTALL_BIN) ./files/esp.init $(1)/etc/init.d/esp
endef

$(eval $(call BuildPackage,esp))

