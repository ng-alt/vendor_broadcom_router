
define eq
$(and $(findstring $(1),$(2)),$(findstring $(2),$(1)))
endef

define kernel-is-version
$(strip \
  $(if $(call eq, $(1), $(LINUX_VERSION)), true, \
    $(if $(call eq, $(1), $(subst .,_,$(LINUX_VERSION))), true)))
endef
