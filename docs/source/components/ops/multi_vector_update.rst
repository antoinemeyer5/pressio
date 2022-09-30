.. include:: ../../mydefs.rst

``multi_vector_update``
=======================

Header: ``<pressio/ops.hpp>``

API
---

.. code-block:: cpp

  namespace pressio { namespace ops{

  template<typename T1, class T2, typename a_t, class b_t>
  void update(T1 & MV, const a_t &a, const T2 & MV1, const b_t &b);

  }} // end namespace pressio::ops

Parameters
----------

* ``MV``: the multiplier used to compute first component of the sum, will be updated with calculated sum

* ``a``: the multiplicand used to compute first component of the sum

* ``MV1``: the multiplier used to compute second component of the sum

* ``b``: the multiplicand used to compute second component of the sum

Constraints
~~~~~~~~~~~

- ``T1`` and ``T2`` must be:

  - an Epetra multi-vector, i.e. ``is_multi_vector_epetra<T>::value`` or

  - a Tpetra multi-vector, i.e. ``pressio::is_multi_vector_tpetra<T>::value`` or

  - a Tpetra multi-vector, i.e. ``pressio::is_multi_vector_tpetra_block<T>::value``

- ``a_t`` and ``b_t`` must be convertible to ``pressio::Traits<T>::scalar_type``

Mandates
~~~~~~~~

:red:`finish`

Preconditions
~~~~~~~~~~~~~

:red:`finish`

Return value
~~~~~~~~~~~~

None

Effects
~~~~~~~

- Computes equation ``MV = a * MV + b * MV1`` and saves the results in ``MV`` object.

- This is a blocking operation, i.e. the kernel completes before returning.

Postconditions
~~~~~~~~~~~~~~

:red:`finish`
