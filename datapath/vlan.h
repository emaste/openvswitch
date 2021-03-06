/*
 * Copyright (c) 2007-2011 Nicira, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef VLAN_H
#define VLAN_H 1

#include <linux/if_vlan.h>
#include <linux/skbuff.h>
#include <linux/version.h>

/**
 * DOC: VLAN tag manipulation.
 *
 * &struct sk_buff handling of VLAN tags has evolved over time:
 *
 * In 2.6.26 and earlier, VLAN tags did not have any generic representation in
 * an skb, other than as a raw 802.1Q header inside the packet data.
 *
 * In 2.6.27 &struct sk_buff added a @vlan_tci member.  Between 2.6.27 and
 * 2.6.32, its value was the raw contents of the 802.1Q TCI field, or zero if
 * no 802.1Q header was present.  This worked OK except for the corner case of
 * an 802.1Q header with an all-0-bits TCI, which could not be represented.
 *
 * In 2.6.33, @vlan_tci semantics changed.  Now, if an 802.1Q header is
 * present, then the VLAN_TAG_PRESENT bit is always set.  This fixes the
 * all-0-bits TCI corner case.
 *
 * For compatibility we emulate the 2.6.33+ behavior on earlier kernel
 * versions.  The client must not access @vlan_tci directly.  Instead, use
 * vlan_get_tci() to read it or vlan_set_tci() to write it, with semantics
 * equivalent to those on 2.6.33+.
 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
#define NEED_VLAN_FIELD
#endif

#ifndef NEED_VLAN_FIELD
static inline void vlan_copy_skb_tci(struct sk_buff *skb) { }

static inline u16 vlan_get_tci(struct sk_buff *skb)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
	if (skb->vlan_tci)
		return skb->vlan_tci | VLAN_TAG_PRESENT;
#endif
	return skb->vlan_tci;
}

static inline void vlan_set_tci(struct sk_buff *skb, u16 vlan_tci)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
	vlan_tci &= ~VLAN_TAG_PRESENT;
#endif
	skb->vlan_tci = vlan_tci;
}
#else
void vlan_copy_skb_tci(struct sk_buff *skb);
u16 vlan_get_tci(struct sk_buff *skb);
void vlan_set_tci(struct sk_buff *skb, u16 vlan_tci);

#undef vlan_tx_tag_present
bool vlan_tx_tag_present(struct sk_buff *skb);

#undef vlan_tx_tag_get
u16 vlan_tx_tag_get(struct sk_buff *skb);

#define __vlan_hwaccel_put_tag rpl__vlan_hwaccel_put_tag
struct sk_buff *__vlan_hwaccel_put_tag(struct sk_buff *skb, u16 vlan_tci);
#endif /* NEED_VLAN_FIELD */

static inline int vlan_deaccel_tag(struct sk_buff *skb)
{
	if (!vlan_tx_tag_present(skb))
		return 0;

	skb = __vlan_put_tag(skb, vlan_tx_tag_get(skb));
	if (unlikely(!skb))
		return -ENOMEM;

	if (get_ip_summed(skb) == OVS_CSUM_COMPLETE)
		skb->csum = csum_add(skb->csum,
				     csum_partial(skb->data + (2 * ETH_ALEN),
						  VLAN_HLEN, 0));

	vlan_set_tci(skb, 0);
	return 0;
}

#endif /* vlan.h */
