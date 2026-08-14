# Central Membrane Stripe Matching

The central membrane stripe matching proceeds by:

1. Making 2D histograms in `(r, phi)` of laser clusters for a measured and reference run after cleaning lamination clusters and noise clusters between rows of stripes.
2. Detecting stripes on the remaining laser clusters with a flood-fill algorithm.
3. Performing an iterative Hungarian algorithm that matches the stripes between the measured and reference samples, then performs a maximum likelihood fit to their displacement with a smooth regularization. The next iteration takes the best global stripe matching assignment that minimizes a combination of the residuals between stripe distances and the ML fit and the fraction of stripes that are left unmatched. After the iteration converges, the resulting smooth displacement fits are the distortion maps in `(r, phi)`.

For static distortions: Field-on Beam-off stripes with Field-off Beam-off reference.

For spacecharge distortions: Field-on Beam-on stripes with Field-on Beam-off reference.

See presentation: [TADistortions_6.10.26.pdf](https://indico.bnl.gov/event/33232/contributions/125677/attachments/70695/121173/TADistortions_6.10.26.pdf)

Updated algorithm ([CMupdates_7.15.26.pdf](https://indico.bnl.gov/event/33573/contributions/127068/attachments/71398/122368/CMupdates_7.15.26.pdf)): Instead of an initial stripe matching seed of mutual nearest neighbors, measured stripes are confined to match only to reference stripes that fall within the same coherent stripe row assignment. This update is largely irrelevant for spacecharge distortions but is more successful for static distortions where the scale of the distortions are on the order of the stripe spacing.

See slide 5 of [CMupdates_7.29.26.pdf](https://indico.bnl.gov/event/33740/contributions/127605/attachments/71714/122924/CMupdates_7.29.26.pdf) for successful extraction of time-averaged `Delta R` and `Delta phi` spacecharge distortions for the first several minutes of each run from our golden Run3 fill.
