#ifndef PBUI_H__PP
#define PBUI_H__PP

#include <vector>
#include <list>
#include <string>

#include <inkview.h>

namespace pocketbook {

	/* PocketBook user interface classes. Simple way for GUI programming with
	 * PocketBook SDK */

	// 1. We must provide signal-slot system for callbacks: */
	struct error: public std::exception { };
	struct calling_of_null_function: public error { };
	struct slot_is_empty: public error {};

	struct signal {
		virtual ~signal() {};
		virtual operator bool () const = 0;
	};

	template <typename R>
	struct signal_0: public signal {
		virtual R operator () () = 0;
		virtual signal_0<R>* clone() const = 0;
	};

	template <typename R, typename T>
	struct signal_1: public signal {
		virtual R operator () (T) = 0;
		virtual signal_1<R, T>* clone() const = 0;
	};

	template <typename R, typename T1, typename T2>
	struct signal_2: public signal {
		virtual R operator () (T1, T2) = 0;
		virtual signal_2<R, T1, T2>* clone() const = 0;
	};

	/* Slots: */

	template <typename R>
	class slot_0 {
		signal_0<R> *sig;

		public:
			slot_0()
			{
				sig = 0;
			}

			slot_0(const signal_0<R>& s)
			{
				sig = s.clone();
			}

			slot_0(const slot_0<R>& s)
			{
				if (s.sig)
					sig = s.sig->clone();
				else
					sig = 0;
			}

			~slot_0()
			{
				delete sig;
			}

			operator bool ()
			{
				if (sig)
					return (bool)(*sig);
				return false;
			}

			void operator = (const slot_0<R> &s)
			{
				if (sig)
					delete sig;

				if (s.sig)
					sig = s.sig->clone();
				else
					sig = 0;
			}

			void operator <<= (const signal_0<R> &s)
			{
				if (sig)
					delete sig;
				sig = s.clone();
			}

			R operator () ()
			{
				if (!sig)
					throw slot_is_empty();
				return (*sig)();
			}
	};

	template <typename R, typename T>
	class slot_1 {
		signal_1<R, T> *sig;

		public:
			slot_1()
			{
				sig = 0;
			}

			slot_1(const signal_1<R, T>& s)
			{
				sig = s.clone();
			}

			slot_1(const slot_1<R, T>& s)
			{
				if (s.sig)
					sig = s.sig->clone();
				else
					sig = 0;
			}

			~slot_1()
			{
				delete sig;
			}

			operator bool ()
			{
				if (sig)
					return (bool)(*sig);
				return false;
			}

			void operator = (const slot_1<R, T> &s)
			{
				if (sig)
					delete sig;

				if (s.sig)
					sig = s.sig->clone();
				else
					sig = 0;
			}

			void operator <<= (const signal_1<R, T> &s)
			{
				if (sig)
					delete sig;
				sig = s.clone();
			}

			R operator () (T t)
			{
				if (!sig)
					throw slot_is_empty();
				return (*sig)(t);
			}
	};

	template <typename R, typename T1, typename T2>
	class slot_2 {
		signal_2<R, T1, T2> *sig;

		public:
			slot_2()
			{
				sig = 0;
			}

			slot_2(const signal_2<R, T1, T2>& s)
			{
				sig = s.clone();
			}

			slot_2(const slot_2<R, T1, T2>& s)
			{
				if (s.sig)
					sig = s.sig->clone();
				else
					sig = 0;
			}

			~slot_2()
			{
				delete sig;
			}

			operator bool ()
			{
				if (sig)
					return (bool)(*sig);
				return false;
			}

			void operator = (const slot_2<R, T1, T2> &s)
			{
				if (sig)
					delete sig;

				if (s.sig)
					sig = s.sig->clone();
				else
					sig = 0;
			}

			void operator <<= (const signal_2<R, T1, T2> &s)
			{
				if (sig)
					delete sig;
				sig = s.clone();
			}

			R operator () (T1 t1, T2 t2)
			{
				if (!sig)
					throw slot_is_empty();
				return (*sig)(t1, t2);
			}
	};

	template <typename R>
	struct func_0: public signal_0<R> {
		func_0()
		{
			fcn = 0;
		}

		func_0(R (*f)())
		{
			fcn = f;
		}

		virtual ~func_0()
		{
		}

		virtual operator bool () const
		{
			return fcn != 0;
		}

		virtual R operator () ()
		{
			if (!fcn)
				throw calling_of_null_function();
			return fcn();
		}

		virtual signal_0<R>* clone() const
		{
			return new func_0<R>(fcn);
		}

		private:
			R (*fcn)();
	};

	template <typename R, typename T>
	struct func_1: public signal_1<R, T> {
		func_1()
		{
			fcn = 0;
		}

		func_1(R (*f)(T))
		{
			fcn = f;
		}

		virtual ~func_1()
		{
		}

		virtual operator bool () const
		{
			return fcn != 0;
		}

		virtual R operator () (T t)
		{
			if (!fcn)
				throw calling_of_null_function();
			return fcn(t);
		}

		virtual signal_1<R, T>* clone() const
		{
			return new func_1<R, T>(fcn);
		}

		private:
			R (*fcn)(T);
	};

	template <typename R, typename T1, typename T2>
	struct func_2: public signal_2<R, T1, T2> {
		func_2()
		{
			fcn = 0;
		}

		func_2(R (*f)(T1, T2))
		{
			fcn = f;
		}

		virtual ~func_2()
		{
		}

		virtual operator bool () const
		{
			return fcn != 0;
		}

		virtual R operator () (T1 t1, T2 t2)
		{
			if (!fcn)
				throw calling_of_null_function();
			return fcn(t1, t2);
		}

		virtual signal_2<R, T1, T2>* clone() const
		{
			return new func_2<R, T1, T2>(fcn);
		}

		private:
			R (*fcn)(T1, T2);
	};

	template <typename R, typename C>
	struct method_0: public signal_0<R> {
		method_0()
		{
			c = 0;
			m = 0;
		}

		method_0(C *self, R (C::*meth)())
		{
			c = self;
			m = meth;
		}

		virtual ~method_0()
		{
		}

		virtual operator bool () const
		{
			return c && m;
		}

		virtual signal_0<R> *clone() const
		{
			return new method_0<R, C>(c, m);
		}

		virtual R operator () ()
		{
			if (!c || !m)
				throw calling_of_null_function();
			return (c->*m)();
		}

		private:
			C *c;
			R (C::*m)();
	};

	template <typename R, typename T, typename C>
	struct method_1: public signal_1<R, T> {
		method_1()
		{
			c = 0;
			m = 0;
		}

		method_1(C *self, R (C::*meth)(T))
		{
			c = self;
			m = meth;
		}

		virtual ~method_1()
		{
		}

		virtual operator bool () const
		{
			return c && m;
		}

		virtual signal_1<R, T> *clone() const
		{
			return new method_1<R, T, C>(c, m);
		}

		virtual R operator () (T t)
		{
			if (!c || !m)
				throw calling_of_null_function();
			return (c->*m)(t);
		}

		private:
			C *c;
			R (C::*m)(T);
	};

	template <typename R, typename T1, typename T2, typename C>
	struct method_2: public signal_2<R, T1, T2> {
		method_2()
		{
			c = 0;
			m = 0;
		}

		method_2(C *self, R (C::*meth)(T1, T2))
		{
			c = self;
			m = meth;
		}

		virtual ~method_2()
		{
		}

		virtual operator bool () const
		{
			return c && m;
		}

		virtual signal_2<R, T1, T2> *clone() const
		{
			return new method_2<R, T1, T2, C>(c, m);
		}

		virtual R operator () (T1 t1, T2 t2)
		{
			if (!c || !m)
				throw calling_of_null_function();
			return (c->*m)(t1, t2);
		}

		private:
			C *c;
			R (C::*m)(T1, T2);
	};

	template <typename T>
	class pointer {
		struct pval {
			T *value;
			int cnt;
		} *value;

		void unref()
		{
			--value->cnt;
			if (value->cnt <= 0) {
				delete value->value;
				value->value = 0;
			}
		}

		void ref()
		{
			++value->cnt;
		}

		void copy(pointer<T>& p)
		{
			if (value)
				delete value;
			value = p.value;
			ref();
		}

		public:
			pointer(T *v = 0)
			{
				value = new pval;
				value->value = v;
				value->cnt = 1;
			}

			pointer(pointer<T> &p)
			{
				value = 0;
				copy(p);
			}

			~pointer()
			{
				unref();
				delete value;
			}

			void operator = (pointer<T> &p)
			{
				copy(p);
			}

			T* operator -> ()
			{
				return value->value;
			}
	};

	// 2. Class, represents PocketBook screen:
	class screen {
		public:
			screen()
			{
			}

			virtual ~screen()
			{
			}

			virtual void draw(unsigned width, unsigned height)
			{
			}

			// Events:
			// TODO: arguments
			slot_2<bool, int, int> init_event;
			slot_2<bool, int, int> exit_event;
			slot_2<bool, int, int> show_event;
			slot_2<bool, int, int> hide_event;
			slot_1<bool, int> key_down_event;
			slot_1<bool, int> key_up_event;
			slot_1<bool, int> key_repeate_event;
			slot_2<bool, int, int> pointer_up_event;
			slot_2<bool, int, int> pointer_down_event;
			slot_2<bool, int, int> pointer_move_event;
			slot_2<bool, int, int> pointer_long_event;
			slot_2<bool, int, int> pointer_hold_event;
			slot_2<bool, int, int> orientation_event;
			slot_2<bool, int, int> snapshot_event;

			slot_2<bool, int, int> prev_page_event;
			slot_2<bool, int, int> next_page_event;
			slot_2<bool, int, int> open_dict_event;

			int event(int type, int par1, int par2)
			{
				switch (type) {
#define EVENT(t, h) \
					case t: \
						if (h##_event) \
							return h##_event(par1, par2)? 1: 0; \
						break
#define EVENT1(t, h) \
					case t: \
						if (h##_event) \
							return h##_event(par1)? 1: 0; \
						break
					EVENT(EVT_INIT, init);
					EVENT(EVT_EXIT, exit);
					EVENT(EVT_SHOW, show);
					EVENT(EVT_HIDE, hide);
					EVENT1(EVT_KEYDOWN, key_down);
					EVENT1(EVT_KEYUP, key_up);
					EVENT1(EVT_KEYREPEAT, key_repeate);
					EVENT(EVT_POINTERUP, pointer_up);
					EVENT(EVT_POINTERDOWN, pointer_down);
					EVENT(EVT_POINTERMOVE, pointer_move);
					EVENT(EVT_POINTERLONG, pointer_long);
					EVENT(EVT_POINTERHOLD, pointer_hold);
					EVENT(EVT_ORIENTATION, orientation);
					EVENT(EVT_SNAPSHOT, snapshot);
					EVENT(EVT_PREVPAGE, prev_page);
					EVENT(EVT_NEXTPAGE, next_page);
					EVENT(EVT_OPENDIC, open_dict);
#undef EVENT
#undef EVENT1
				};

				return 0;
			}
	};
}

#endif

