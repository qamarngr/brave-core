import * as React from 'react'

export default function useOnScreen<T extends Element> (
  ref: React.MutableRefObject<T>,
  options: IntersectionObserverInit = { rootMargin: '0px' }
): boolean {
  const [isIntersecting, setIsIntersecting] = React.useState<boolean>(false)

  React.useEffect(() => {
    const observer = new IntersectionObserver(([entry]) => {
      setIsIntersecting(entry.isIntersecting)
    }, options)

    if (ref.current) {
      observer.observe(ref.current)
    }

    return () => {
      if (ref.current) {
        observer.unobserve(ref.current)
      }
    }
  }, [ref])

  return isIntersecting
}
