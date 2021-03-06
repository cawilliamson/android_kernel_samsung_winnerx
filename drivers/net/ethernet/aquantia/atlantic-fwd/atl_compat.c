/*
 * aQuantia Corporation Network Driver
 * Copyright (C) 2017 aQuantia Corporation. All rights reserved
 *
 * Portions Copyright (C) various contributors (see specific commit references)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 */

#include "atl_common.h"
#include "atl_ring.h"
#include <linux/msi.h>
#include <linux/cpu.h>
#include <linux/interrupt.h>

#ifdef ATL_COMPAT_PCI_ALLOC_IRQ_VECTORS

/* From commit aff171641d181ea573380efc3f559c9de4741fc5 */
int atl_compat_pci_irq_vector(struct pci_dev *dev, unsigned int nr)
{
#ifdef CONFIG_PCI_MSI
	if (dev->msix_enabled) {
		struct msi_desc *entry;
		int i = 0;

		for_each_pci_msi_entry(entry, dev) {
			if (i == nr)
				return entry->irq;
			i++;
		}
		WARN_ON_ONCE(1);
		return -EINVAL;
	}

	if (dev->msi_enabled) {
		struct msi_desc *entry = first_pci_msi_entry(dev);

		if (WARN_ON_ONCE(nr >= entry->nvec_used))
			return -EINVAL;
	} else {
		if (WARN_ON_ONCE(nr > 0))
			return -EINVAL;
	}
#endif

	return dev->irq + nr;
}

int atl_compat_pci_alloc_irq_vectors(struct pci_dev *dev,
	unsigned int min_vecs, unsigned int max_vecs, unsigned int flags)
{
	int vecs = -ENOSPC;

	if (flags & PCI_IRQ_MSIX) {
		struct msix_entry *entries;
		int i;

		entries = kcalloc(max_vecs, sizeof(*entries), GFP_KERNEL);
		if (!entries)
			return -ENOMEM;

		for (i = 0; i < max_vecs; i++)
			entries[i].entry = i;

		vecs = pci_enable_msix_range(dev, entries, min_vecs, max_vecs);
		kfree(entries);
		if (vecs > 0)
			return vecs;
	}

	if (flags & PCI_IRQ_MSI) {
		vecs = pci_enable_msi_range(dev, min_vecs, max_vecs);
		if (vecs > 0)
			return vecs;
	}

	/* use legacy irq if allowed */
	if ((flags & PCI_IRQ_LEGACY) && min_vecs == 1) {
		pci_intx(dev, 1);
		return 1;
	}

	return vecs;
}

#endif
