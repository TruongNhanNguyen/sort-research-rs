extern "C" {
    fn pdqsort_unstable_i32(data: *mut i32, len: usize);
    fn pdqsort_unstable_i32_by(
        data: *mut i32,
        len: usize,
        cmp_fn: unsafe extern "C" fn(&i32, &i32, *mut u8) -> bool,
        cmp_fn_ctx: *mut u8,
    );
    fn pdqsort_unstable_u64(data: *mut u64, len: usize);
    fn pdqsort_unstable_u64_by(
        data: *mut u64,
        len: usize,
        cmp_fn: unsafe extern "C" fn(&u64, &u64, *mut u8) -> bool,
        cmp_fn_ctx: *mut u8,
    );
}

use std::cmp::Ordering;

use crate::ffi_util::rust_fn_cmp;

trait PdqSort: Sized {
    fn sort(data: &mut [Self]);
    fn sort_by<F: FnMut(&Self, &Self) -> Ordering>(data: &mut [Self], compare: F);
}

impl<T> PdqSort for T {
    default fn sort(_data: &mut [T]) {
        panic!("Type not supported");
    }

    default fn sort_by<F: FnMut(&T, &T) -> Ordering>(_data: &mut [T], _compare: F) {
        panic!("Type not supported");
    }
}

impl PdqSort for i32 {
    fn sort(data: &mut [i32]) {
        unsafe {
            pdqsort_unstable_i32(data.as_mut_ptr(), data.len());
        }
    }

    fn sort_by<F: FnMut(&i32, &i32) -> Ordering>(data: &mut [i32], compare: F) {
        make_libcxx_sort_by!(pdqsort_unstable_i32_by, data, compare, i32);
    }
}

impl PdqSort for u64 {
    fn sort(data: &mut [u64]) {
        unsafe {
            pdqsort_unstable_u64(data.as_mut_ptr(), data.len());
        }
    }

    fn sort_by<F: FnMut(&u64, &u64) -> Ordering>(data: &mut [u64], compare: F) {
        make_libcxx_sort_by!(pdqsort_unstable_u64_by, data, compare, u64);
    }
}

pub fn sort<T: Ord>(data: &mut [T]) {
    PdqSort::sort(data);
}

pub fn sort_by<T, F: FnMut(&T, &T) -> Ordering>(data: &mut [T], compare: F) {
    PdqSort::sort_by(data, compare);
}
